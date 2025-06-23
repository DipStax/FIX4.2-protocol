import tkinter as tk
from tkinter import ttk, messagebox
import tkinter.filedialog as fd
import os
import random
from typing import Any, List, Tuple, Dict, Union
from datetime import datetime
import re

class SingletonMeta(type):
    _instances = {}

    def __call__(cls, *args, **kwargs):
        if cls not in cls._instances:
            cls._instances[cls] = super().__call__(*args, **kwargs)
        return cls._instances[cls]

class LogFileInfo:
    def __init__(self, path: str, relative_to: str) -> None:
        self.abs_path = path
        self.rel_path = os.path.relpath(path, relative_to)

        self.color: str = "#"+("%06x" % random.randint(0, 255 ** 3))
        self.showed: bool = True

    def __str__(self):
        return f"LogFileInfo{{ AbsPath: {self.abs_path}, RelPath: {self.rel_path}, Color: {self.color}, Showed: {self.showed} }}"

class LogFolderInfo:
    def __init__(self, path: str, relative_to: str) -> None:
        self.abs_path = path
        self.rel_path = os.path.relpath(path, relative_to)

        self.showed: bool = True

    def __str__(self):
        return f"LogFolderInfo{{ AbsPath: {self.abs_path}, RelPath: {self.rel_path}, Showed: {self.showed} }}"

class LogViewerInstance(metaclass=SingletonMeta):
    def __init__(self) -> None:
        self.inc_file: List[LogFileInfo] = []
        self.exc_file: List[LogFileInfo] = []

    def include(self, files: List[LogFileInfo]) -> None:
        for file in files:
            if file in self.exc_file:
                self.exc_file.remove(file)
            if file not in self.inc_file:
                file.showed = True
                print(f"Including file: {file}")
                self.inc_file.append(file)

    def exclude(self, files: List[LogFileInfo]) -> None:
        for file in files:
            if file in self.inc_file:
                self.inc_file.remove(file)
            if file not in self.exc_file:
                file.showed = False
                print(f"Excluding file: {file}")
                self.exc_file.append(file)

class TopLevelMenu:
    OpenProjectFolderEvent: str = "<<open-project-folder>>"

    def __init__(self, root: tk.Tk) -> None:
        self.root: tk.Tk = root
        self.root_menu = tk.Menu(root)

        project_menu = tk.Menu(self.root_menu)
        project_menu.add_command(label="Open folder", command=self.open_folder)

        self.root_menu.add_cascade(label="Project", menu=project_menu)

        self.root.config(menu=self.root_menu)

    def open_folder(self) -> None:
        self.root.event_generate(TopLevelMenu.OpenProjectFolderEvent)

class ProjectExplorer(tk.Frame):
    RefreshLogViewDisplayEvent: str = "<<refresh-log-view-display>>"

    def __init__(self, parent, **kwargs):
        super().__init__(parent, **kwargs)

        self.root = parent

        self.refresh_button: tk.Button = tk.Button(self, text="refresh")
        self.refresh_button.pack(side=tk.BOTTOM, fill=tk.X)

        self.item_info: Dict[str, Union[LogFileInfo, LogFolderInfo]] = {}

        self.tree = ttk.Treeview(self)
        self.tree.pack(fill=tk.BOTH, expand=True, padx=2, pady=2)
        self.tree.bind("<Button-3>", self.open_contextmenu)

    def open_contextmenu(self, event) -> None:
        selected_item: Tuple[str, ...] = self.tree.selection()

        if len(selected_item) == 1:
            if type(self.item_info[selected_item[0]]) is not LogFolderInfo:
                cm_singleselect = tk.Menu(self)
                cm_singleselect.add_command(label="Include", command=lambda: self.include_single(selected_item[0]))
                cm_singleselect.add_command(label="Exclude", command=lambda: self.exclude_single(selected_item[0]))
                cm_singleselect.add_command(label="Configure")

                cm_singleselect.tk_popup(event.x_root, event.y_root)
            else:
                messagebox.showwarning("Error in selection", "You cannot select a folder")
        else:
            is_folder: bool = False
            list_folder: List[str] = []

            for item in selected_item:
                if type(self.item_info[item]) is LogFolderInfo:
                    list_folder.append(self.tree.item(item, "text"))
                    is_folder = True

            if is_folder:
                messagebox.showwarning("Error in selection", f"You cannot select a folder: " + ", ".join(list_folder))
            else:
                cm_multiselect = tk.Menu(self)
                cm_multiselect.add_command(label="Includes selection", command=lambda: self.include_multiple(selected_item))
                cm_multiselect.add_command(label="Excludes selection", command=lambda: self.exclude_multiple(selected_item))

                cm_multiselect.tk_popup(event.x_root, event.y_root)

    def open_project(self, _) -> None:
        path: str = fd.askdirectory(parent=self, title="Open log project")
        track_files: List[str] = []

        for root, _, files in os.walk(path):
            for file in files:
                track_files.append(os.path.join(root, file))

        common_path: str = os.path.commonpath(track_files)
        for filepath in track_files:
            relative_path: str = os.path.relpath(filepath, common_path)
            part_path: List[str] = relative_path.strip(os.sep).split(os.sep)
            current: str = ""
            for index, part in enumerate(part_path):
                children: Tuple[str, ...] = self.tree.get_children(current)
                found: None | Any  = None
                for child in children:
                    if self.tree.item(child, "text") == part:
                        found = child
                        break
                if found:
                    current = found
                else:
                    current = self.tree.insert(current, "end", text=part, open=True)
                    if index == len(part_path) - 1:
                        self.item_info[current] = LogFileInfo(relative_path, common_path)
                    else:
                        self.item_info[current] = LogFolderInfo(relative_path, common_path)

    def include_single(self, selected: str) -> None:
        self.include_multiple([selected])

    def include_multiple(self, selected: str) -> None:
        viewer: LogViewerInstance = LogViewerInstance()
        viewer.include([self.item_info[file_selected] for file_selected in selected])
        self.root.event_generate(ProjectExplorer.RefreshLogViewDisplayEvent)

    def exclude_single(self, selected: str) -> None:
        self.exclude_multiple([selected])

    def exclude_multiple(self, selected: List[str]) -> None:
        viewer: LogViewerInstance = LogViewerInstance()
        viewer.exclude([self.item_info[file_selected] for file_selected in selected])
        self.root.event_generate(ProjectExplorer.RefreshLogViewDisplayEvent)

class LogViewer(tk.Frame):
    class LogLineInfo:
        def __init__(self, line: str, color: str) -> None:
            self.line: str = line
            self.color: str = color
            self.valid: bool =  False

            match = re.compile(
                r"\[(?P<level>\w+)]\s"
                r"\((?P<address>[^)]+)\)\s"
                r"(?P<time>\d{4}-\d{2}-\d{2} \d{2}:\d{2}:\d{2})\s-\s"
                r"(?P<message>.+)"
            ).match(line)
            if not match:
                return

            self.level: str = match.group("level")
            self.address: str = match.group("address")
            self.timestamp: str = datetime.strptime(match.group("time"), "%Y-%m-%d %H:%M:%S")
            self.message: str = match.group("message").replace("\u0001", "|")
            self.valid =  True

    def __init__(self, parent, **kwargs) -> None:
        super().__init__(parent, **kwargs)

        self.lines: List[LogViewer.LogLineInfo] = []

        scrollbar = tk.Scrollbar(self)
        scrollbar.pack(side=tk.RIGHT, fill=tk.Y)

        self.log_text = tk.Text(self, yscrollcommand=scrollbar.set)
        self.log_text.pack(side=tk.LEFT, fill=tk.BOTH, expand=True)
        self.log_text.config(state="disabled")

        scrollbar.config(command=self.log_text.yview)

        parent.bind(ProjectExplorer.RefreshLogViewDisplayEvent, self.refresh_display)

    def refresh_display(self, _) -> None:
        logconf: LogViewerInstance = LogViewerInstance()

        for inc_file in logconf.inc_file:
            with open(inc_file.abs_path) as file:
                for line in file:
                    line_info: LogFileInfo.LogLineInfo = LogFileInfo.LogLineInfo(line.rstrip(), inc_file.color)
                    if line_info.valid:
                        self.lines.append(line_info)

        self.lines.sort(key=lambda log: log.timestamp)
        self.update_display()

    def update_display(self):
        self.log_text.config(state="normal")
        self.log_text.delete("1.0", tk.END)

        for index, line in enumerate(self.lines):
            tag_name: str = f"addr_{index}"
            log_text: str = f"[{line.timestamp.strftime('%Y-%m-%d %H:%M:%S')}] - [{line.level}] ({line.address}) {line.message}\n"

            self.log_text.insert(tk.END, log_text)
            self.log_text.tag_add(tag_name, f"{index}.0", f"{index}.end")
            self.log_text.tag_config(tag_name, background=line.color)

        self.log_text.config(state="disabled")

if __name__ == "__main__":
    root: tk.Tk = tk.Tk()

    tl_menu: TopLevelMenu = TopLevelMenu(root)

    main: tk.Frame = tk.Frame(root)
    main.pack(fill=tk.BOTH, expand=True)

    panedwin: tk.PanedWindow = tk.PanedWindow(main, orient=tk.HORIZONTAL)
    panedwin.pack(fill=tk.BOTH, expand=True)

    project_explorer: ProjectExplorer = ProjectExplorer(panedwin, background="blue")
    log_viewer: LogViewer = LogViewer(panedwin, background="red")

    root.bind(TopLevelMenu.OpenProjectFolderEvent, project_explorer.open_project)

    panedwin.add(project_explorer)
    panedwin.add(log_viewer)

    root.mainloop()