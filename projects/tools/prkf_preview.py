"""Preview PRKF animations and melee hitboxes without running the game.

Run:
    py prkf_preview.py [path/to/animation.prkf]

The viewer discovers image frames in the PRKF file's directory. PRKF delays use
the same centisecond conversion as SpriteAnimation::Update.
"""

from __future__ import annotations

import argparse
import math
import re
import time
import tkinter as tk
from dataclasses import dataclass, field
from pathlib import Path
from tkinter import filedialog, messagebox, ttk

try:
    from PIL import Image, ImageTk
except ImportError:
    Image = None
    ImageTk = None


@dataclass
class HitboxEvent:
    offset_x: float
    offset_y: float
    width: float
    height: float


@dataclass
class TransformEvent:
    offset_x: float = 0.0
    offset_y: float = 0.0
    scale: float = 1.0
    rotation_deg: float = 0.0


@dataclass
class Keyframe:
    texture_index: int
    delay: float
    hitboxes: list[HitboxEvent] = field(default_factory=list)
    transform: TransformEvent = field(default_factory=TransformEvent)
    source_line: int = -1


def natural_key(path: Path) -> list[tuple[int, int | str]]:
    return [(0, int(part)) if part.isdigit() else (1, part.lower())
            for part in re.split(r"(\d+)", path.stem)]


def parse_prkf(path: Path) -> tuple[list[list[Keyframe]], list[str]]:
    animations: list[list[Keyframe]] = []
    current: list[Keyframe] = []
    warnings: list[str] = []

    for line_number, raw_line in enumerate(path.read_text(encoding="utf-8").splitlines(), 1):
        line = raw_line.strip()
        if not line:
            if current:
                animations.append(current)
                current = []
            continue

        segments = [segment.strip() for segment in line.split("|")]
        fields = segments[0].split()
        if len(fields) != 2:
            warnings.append(f"Line {line_number}: expected texture index and delay")
            continue

        try:
            keyframe = Keyframe(int(fields[0]), float(fields[1]), source_line=line_number - 1)
        except ValueError:
            warnings.append(f"Line {line_number}: invalid texture index or delay")
            continue

        for segment in segments[1:]:
            event_fields = segment.split()
            if not event_fields:
                warnings.append(f"Line {line_number}: empty event")
                continue
            if event_fields[0] == "transform":
                if len(event_fields) != 5:
                    warnings.append(f"Line {line_number}: transform needs x y scale rotationDeg")
                    continue
                try:
                    keyframe.transform = TransformEvent(
                        *(float(value) for value in event_fields[1:])
                    )
                except ValueError:
                    warnings.append(f"Line {line_number}: invalid transform number")
                continue
            if event_fields[0] != "melee_hitbox":
                warnings.append(f"Line {line_number}: unknown event '{event_fields[0]}'")
                continue
            if len(event_fields) != 5:
                warnings.append(f"Line {line_number}: melee_hitbox needs x y width height")
                continue
            try:
                keyframe.hitboxes.append(HitboxEvent(*(float(value) for value in event_fields[1:])))
            except ValueError:
                warnings.append(f"Line {line_number}: invalid melee_hitbox number")

        current.append(keyframe)

    if current:
        animations.append(current)
    return animations, warnings


def save_transforms(path: Path, animations: list[list[Keyframe]], changed_frames: set[tuple[int, int]]) -> None:
    lines = path.read_bytes().decode("utf-8").splitlines(keepends=True)
    for block_index, animation in enumerate(animations):
        for frame_index, keyframe in enumerate(animation):
            if (block_index, frame_index) not in changed_frames:
                continue
            line = lines[keyframe.source_line]
            content = line.rstrip("\r\n")
            ending = line[len(content):]
            segments = content.split("|")
            transform = keyframe.transform
            replacement = (f" transform {transform.offset_x:g} {transform.offset_y:g} "
                           f"{transform.scale:g} {transform.rotation_deg:g}")
            for index in range(1, len(segments)):
                if segments[index].strip().split()[:1] == ["transform"]:
                    segments[index] = replacement + segments[index][len(segments[index].rstrip()):]
                    break
            else:
                if not segments[-1].endswith(" "):
                    segments[-1] += " "
                segments.append(replacement)
            lines[keyframe.source_line] = "|".join(segments) + ending
    path.write_bytes("".join(lines).encode("utf-8"))


class PrkfPreview:
    def __init__(self, root: tk.Tk) -> None:
        self.root = root
        self.root.title("PRKF Animation Preview")
        self.root.geometry("1000x760")
        self.root.minsize(720, 540)

        self.path: Path | None = None
        self.animations: list[list[Keyframe]] = []
        self.image_paths: list[Path] = []
        self.images: list[tk.PhotoImage] = []
        self.pillow_images: list[object] = []
        self.scaled_image: tk.PhotoImage | None = None
        self.frame_index = 0
        self.elapsed = 0.0
        self.last_tick = time.perf_counter()

        self.playing = tk.BooleanVar(value=True)
        self.looping = tk.BooleanVar(value=True)
        self.show_hitboxes = tk.BooleanVar(value=True)
        self.show_origin = tk.BooleanVar(value=True)
        self.zoom = tk.IntVar(value=3)
        self.speed = tk.DoubleVar(value=1.0)
        self.animation_block = tk.IntVar(value=0)
        self.status = tk.StringVar(value="Open a .prkf file")
        self.frame_text = tk.StringVar(value="No animation loaded")
        self.transform_values = [tk.StringVar() for _ in range(4)]
        self.displayed_frame: tuple[int, int] | None = None
        self.changed_frames: set[tuple[int, int]] = set()
        self.invalid_transform = False
        self.syncing_transform = False
        for value in self.transform_values:
            value.trace_add("write", self._transform_changed)

        self._build_ui()
        self.root.after(10, self._tick)

    def _build_ui(self) -> None:
        toolbar = ttk.Frame(self.root, padding=8)
        toolbar.pack(fill=tk.X)
        ttk.Button(toolbar, text="Open PRKF", command=self.open_dialog).pack(side=tk.LEFT)
        self.save_button = ttk.Button(toolbar, text="Save", command=self.save, state=tk.DISABLED)
        self.save_button.pack(side=tk.LEFT, padx=(4, 0))
        ttk.Button(toolbar, text="Previous", command=lambda: self.step(-1)).pack(side=tk.LEFT, padx=(8, 0))
        ttk.Button(toolbar, text="Next", command=lambda: self.step(1)).pack(side=tk.LEFT, padx=(4, 0))
        ttk.Checkbutton(toolbar, text="Play", variable=self.playing).pack(side=tk.LEFT, padx=(12, 0))
        ttk.Checkbutton(toolbar, text="Loop", variable=self.looping).pack(side=tk.LEFT, padx=(8, 0))
        ttk.Checkbutton(
            toolbar, text="Hitboxes", variable=self.show_hitboxes, command=self.redraw
        ).pack(side=tk.LEFT, padx=(8, 0))
        ttk.Checkbutton(
            toolbar, text="Origin", variable=self.show_origin, command=self.redraw
        ).pack(side=tk.LEFT, padx=(8, 0))

        settings = ttk.Frame(self.root, padding=(8, 0, 8, 8))
        settings.pack(fill=tk.X)
        ttk.Label(settings, text="Animation block").pack(side=tk.LEFT)
        self.block_selector = ttk.Combobox(
            settings, width=5, state="readonly", textvariable=self.animation_block
        )
        self.block_selector.pack(side=tk.LEFT, padx=(5, 15))
        self.block_selector.bind("<<ComboboxSelected>>", self._select_block)
        ttk.Label(settings, text="Zoom").pack(side=tk.LEFT)
        ttk.Spinbox(
            settings, from_=1, to=10, width=4, textvariable=self.zoom,
            command=self.redraw
        ).pack(side=tk.LEFT, padx=(5, 15))
        ttk.Label(settings, text="Speed").pack(side=tk.LEFT)
        speed_box = ttk.Combobox(
            settings, width=6, state="readonly", textvariable=self.speed,
            values=(0.25, 0.5, 1.0, 2.0, 4.0)
        )
        speed_box.pack(side=tk.LEFT, padx=(5, 15))
        ttk.Label(settings, textvariable=self.frame_text).pack(side=tk.LEFT)

        transform_controls = ttk.Frame(self.root, padding=(8, 0, 8, 8))
        transform_controls.pack(fill=tk.X)
        ttk.Label(transform_controls, text="Frame transform").pack(side=tk.LEFT, padx=(0, 12))
        for label, value in zip(("X", "Y", "Scale", "Rotation (deg)"), self.transform_values):
            ttk.Label(transform_controls, text=label).pack(side=tk.LEFT)
            entry = ttk.Entry(transform_controls, width=9, textvariable=value)
            entry.pack(side=tk.LEFT, padx=(4, 12))
            entry.bind("<FocusIn>", lambda _event: self.playing.set(False))

        self.canvas = tk.Canvas(
            self.root, background="#24272d", highlightthickness=0
        )
        self.canvas.pack(fill=tk.BOTH, expand=True, padx=8)
        self.canvas.bind("<Configure>", lambda _event: self.redraw())

        timeline = ttk.Frame(self.root, padding=8)
        timeline.pack(fill=tk.X)
        self.frame_slider = ttk.Scale(
            timeline, from_=0, to=0, orient=tk.HORIZONTAL, command=self._seek
        )
        self.frame_slider.pack(fill=tk.X)
        ttk.Label(self.root, textvariable=self.status, padding=(8, 0, 8, 8)).pack(fill=tk.X)

        self.root.bind("<space>", lambda event: self.playing.set(not self.playing.get())
                   if not isinstance(event.widget, ttk.Entry) else None)
        self.root.bind("<Left>", lambda event: self.step(-1)
                   if not isinstance(event.widget, ttk.Entry) else None)
        self.root.bind("<Right>", lambda event: self.step(1)
                   if not isinstance(event.widget, ttk.Entry) else None)
        self.root.protocol("WM_DELETE_WINDOW", self.close)

    def open_dialog(self) -> None:
        if self.changed_frames and not messagebox.askyesno(
            "Discard edits?", "Open another PRKF and discard unsaved transform edits?"
        ):
            return
        selected = filedialog.askopenfilename(
            title="Open PRKF animation",
            filetypes=(("PRKF animation", "*.prkf"), ("All files", "*.*")),
        )
        if selected:
            self.load(Path(selected))

    def close(self) -> None:
        if self.changed_frames and not messagebox.askyesno(
            "Discard edits?", "Close without saving transform edits?"
        ):
            return
        self.root.destroy()

    def load(self, path: Path) -> None:
        try:
            animations, warnings = parse_prkf(path)
            if not animations:
                raise ValueError("No valid animation keyframes were found")

            image_paths = sorted(
                (candidate for candidate in path.parent.iterdir()
                 if candidate.suffix.lower() in {".png", ".gif", ".ppm", ".pgm"}),
                key=natural_key,
            )
            if not image_paths:
                raise ValueError("No Tk-compatible image frames were found beside the PRKF file")

            images = [tk.PhotoImage(file=str(image_path)) for image_path in image_paths]
            pillow_images = (
                [Image.open(image_path).convert("RGBA") for image_path in image_paths]
                if Image is not None else []
            )
        except (OSError, tk.TclError, ValueError) as error:
            messagebox.showerror("Could not open animation", str(error))
            return

        self.path = path
        self.animations = animations
        self.image_paths = image_paths
        self.images = images
        self.pillow_images = pillow_images
        self.animation_block.set(0)
        self.block_selector["values"] = tuple(range(len(animations)))
        self.frame_index = 0
        self.elapsed = 0.0
        self.playing.set(True)
        self.displayed_frame = None
        self.changed_frames.clear()
        self.invalid_transform = False
        self.save_button.configure(state=tk.DISABLED)
        self._configure_timeline()
        warning_text = f" | {len(warnings)} warning(s)" if warnings else ""
        self.status.set(
            f"{path} | {len(images)} image(s), {len(animations)} block(s){warning_text}"
        )
        if warnings:
            self.status.set(self.status.get() + " | " + "; ".join(warnings[:3]))
        self.redraw()

    def _sync_transform_fields(self, keyframe: Keyframe) -> None:
        identity = (self.animation_block.get(), self.frame_index)
        if identity == self.displayed_frame:
            return
        self.displayed_frame = identity
        self.invalid_transform = False
        self.syncing_transform = True
        for value, number in zip(self.transform_values, (
            keyframe.transform.offset_x, keyframe.transform.offset_y,
            keyframe.transform.scale, keyframe.transform.rotation_deg,
        )):
            value.set(f"{number:g}")
        self.syncing_transform = False

    def _transform_changed(self, *_args: object) -> None:
        if self.syncing_transform or self.displayed_frame != (self.animation_block.get(), self.frame_index):
            return
        try:
            values = [float(value.get()) for value in self.transform_values]
        except ValueError:
            self.invalid_transform = True
            return
        if not all(math.isfinite(value) for value in values) or values[2] < 0:
            self.invalid_transform = True
            return
        self.invalid_transform = False
        keyframe = self.current_animation()[self.frame_index]
        transform = keyframe.transform
        if tuple(values) != (transform.offset_x, transform.offset_y,
                     transform.scale, transform.rotation_deg):
            keyframe.transform = TransformEvent(*values)
            self.changed_frames.add(self.displayed_frame)
            self.save_button.configure(state=tk.NORMAL)
            self.redraw()

    def save(self) -> None:
        if self.path is None or not self.changed_frames:
            return
        if self.invalid_transform:
            messagebox.showerror("Invalid transform", "Enter finite numbers and a nonnegative scale.")
            return
        try:
            save_transforms(self.path, self.animations, self.changed_frames)
        except (OSError, UnicodeError) as error:
            messagebox.showerror("Could not save animation", str(error))
            return
        self.changed_frames.clear()
        self.save_button.configure(state=tk.DISABLED)
        self.status.set(f"Saved {self.path}")

    def current_animation(self) -> list[Keyframe]:
        if not self.animations:
            return []
        block = min(self.animation_block.get(), len(self.animations) - 1)
        return self.animations[block]

    def _configure_timeline(self) -> None:
        animation = self.current_animation()
        self.frame_slider.configure(to=max(0, len(animation) - 1))
        self.frame_slider.set(self.frame_index)

    def _select_block(self, _event: object = None) -> None:
        self.frame_index = 0
        self.elapsed = 0.0
        self._configure_timeline()
        self.redraw()

    def _seek(self, value: str) -> None:
        animation = self.current_animation()
        if not animation:
            return
        frame = max(0, min(round(float(value)), len(animation) - 1))
        if frame != self.frame_index:
            self.frame_index = frame
            self.elapsed = 0.0
            self.redraw()

    def step(self, amount: int) -> None:
        animation = self.current_animation()
        if not animation:
            return
        self.playing.set(False)
        self.frame_index = (self.frame_index + amount) % len(animation)
        self.elapsed = 0.0
        self.frame_slider.set(self.frame_index)
        self.redraw()

    def _tick(self) -> None:
        now = time.perf_counter()
        delta = now - self.last_tick
        self.last_tick = now
        animation = self.current_animation()
        if self.playing.get() and animation:
            self.elapsed += delta * self.speed.get()
            delay_seconds = animation[self.frame_index].delay / 100.0
            if delay_seconds > 0.0 and self.elapsed >= delay_seconds:
                self.elapsed %= delay_seconds
                if self.frame_index + 1 < len(animation):
                    self.frame_index += 1
                elif self.looping.get():
                    self.frame_index = 0
                else:
                    self.playing.set(False)
                self.frame_slider.set(self.frame_index)
                self.redraw()
        self.root.after(10, self._tick)

    def redraw(self) -> None:
        self.canvas.delete("all")
        animation = self.current_animation()
        if not animation or not self.images:
            return

        keyframe = animation[self.frame_index]
        self._sync_transform_fields(keyframe)
        if keyframe.texture_index < 0 or keyframe.texture_index >= len(self.images):
            self.frame_text.set(
                f"Frame {self.frame_index + 1}/{len(animation)}: missing texture {keyframe.texture_index}"
            )
            return

        zoom = max(1, min(10, self.zoom.get()))
        source_image = self.images[keyframe.texture_index]
        center_x = self.canvas.winfo_width() * 0.5
        center_y = self.canvas.winfo_height() * 0.5
        transform = keyframe.transform
        image_center_x = center_x + transform.offset_x * zoom
        image_center_y = center_y + transform.offset_y * zoom

        grid_size = 16 * zoom
        for x in range(int(center_x) % grid_size, self.canvas.winfo_width(), grid_size):
            self.canvas.create_line(x, 0, x, self.canvas.winfo_height(), fill="#2e3239")
        for y in range(int(center_y) % grid_size, self.canvas.winfo_height(), grid_size):
            self.canvas.create_line(0, y, self.canvas.winfo_width(), y, fill="#2e3239")

        if self.pillow_images and ImageTk is not None:
            pillow_image = self.pillow_images[keyframe.texture_index]
            transformed_width = max(1, round(pillow_image.width * zoom * max(0.0, transform.scale)))
            transformed_height = max(1, round(pillow_image.height * zoom * max(0.0, transform.scale)))
            transformed_image = pillow_image.resize(
                (transformed_width, transformed_height), Image.Resampling.NEAREST
            )
            transformed_image = transformed_image.rotate(
                -transform.rotation_deg,
                resample=Image.Resampling.NEAREST,
                expand=True,
            )
            self.scaled_image = ImageTk.PhotoImage(transformed_image)
        else:
            integer_scale = max(1, round(zoom * max(0.0, transform.scale)))
            self.scaled_image = source_image.zoom(integer_scale, integer_scale)

        self.canvas.create_image(image_center_x, image_center_y, image=self.scaled_image)

        if self.show_hitboxes.get():
            for index, hitbox in enumerate(keyframe.hitboxes, 1):
                hitbox_center_x = center_x + hitbox.offset_x * zoom
                hitbox_center_y = center_y + hitbox.offset_y * zoom
                half_width = hitbox.width * zoom * 0.5
                half_height = hitbox.height * zoom * 0.5
                self.canvas.create_rectangle(
                    hitbox_center_x - half_width,
                    hitbox_center_y - half_height,
                    hitbox_center_x + half_width,
                    hitbox_center_y + half_height,
                    outline="#ff4545",
                    width=2,
                    fill="#5a3038",
                    stipple="gray50",
                )
                self.canvas.create_text(
                    hitbox_center_x - half_width + 4,
                    hitbox_center_y - half_height + 4,
                    text=f"H{index}", anchor=tk.NW, fill="#ff8c8c"
                )

        if self.show_origin.get():
            self.canvas.create_line(center_x - 10, center_y, center_x + 10, center_y, fill="#62d9ff")
            self.canvas.create_line(center_x, center_y - 10, center_x, center_y + 10, fill="#62d9ff")

        image_name = self.image_paths[keyframe.texture_index].name
        self.frame_text.set(
            f"Frame {self.frame_index + 1}/{len(animation)} | texture {keyframe.texture_index} "
            f"({image_name}) | {keyframe.delay:g} cs | {len(keyframe.hitboxes)} hitbox(es) | "
            f"transform {transform.offset_x:g} {transform.offset_y:g} "
            f"{transform.scale:g} {transform.rotation_deg:g}"
        )


def main() -> None:
    parser = argparse.ArgumentParser(description="Preview PRKF sprite animations and hitboxes.")
    parser.add_argument("prkf", nargs="?", type=Path, help="PRKF file to open")
    args = parser.parse_args()

    root = tk.Tk()
    preview = PrkfPreview(root)
    if args.prkf:
        preview.load(args.prkf.resolve())
    root.mainloop()


if __name__ == "__main__":
    main()