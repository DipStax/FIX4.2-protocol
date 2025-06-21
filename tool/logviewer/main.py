import tkinter as tk
from tkinter import ttk
import tkinter.filedialog as fd
import json
import os
import random
from datetime import datetime
import re

from typing import Dict, List, Set

class SingletonMeta(type):
    _instances = {}

    def __call__(cls, *args, **kwargs):
        if cls not in cls._instances:
            cls._instances[cls] = super().__call__(*args, **kwargs)
        return cls._instances[cls]

class LogConfiguration(metaclass=SingletonMeta):
    def __init__(self) -> None:
        self.inc_folder: List[str] = []
        self.exc_folder: List[str] = []

        self.inc_file: List[str] = []
        self.exc_file: List[str] = []

    @staticmethod
    def _manage(remove: List[str], keep: List[str], data: str) -> bool:
        if data in remove:
            remove.remove(data)
        if data in keep:
            return False
        keep.append(data)
        return True

    def include_folder(self, folder: str) -> bool:
        return self._manage(self.exc_folder, self.inc_folder, folder)

    def exclude_folder(self, folder: str) -> bool:
        return self._manage(self.inc_folder, self.exc_folder, folder)

    def include_file(self, file: str) -> bool:
        return self._manage(self.exc_file, self.inc_file, file)

    def exclude_file(self, file: str) -> None:
        return self._manage(self.inc_file, self.exc_file, file)

    def __str__(self) -> str:
        data: Dict[str, List[str]] = {
            "Include folder": self.inc_folder,
            "Exclude folder": self.exc_folder,
            "Include file": self.inc_file,
            "Exclude file": self.exc_file,
        }
        return json.dumps(data, indent=2)


class TopLevelMenu:
    RefreshLogConfigEvent: str = "<<refresh-log-config>>"

    def __init__(self, root: tk.Tk) -> None:
        self.toplevel_menu = tk.Menu(root)

        file_menu = tk.Menu(self.toplevel_menu)
        file_menu.add_command(label="Include logs folder", command=self.include_folder)
        file_menu.add_command(label="Exclude logs folder", command=self.exclude_folder)
        # file_menu.add_command(label="Include logs file", command=self.include_file)
        # file_menu.add_command(label="Exclude logs file", command=self.exclude_file)

        self.toplevel_menu.add_cascade(label="File", menu=file_menu)

        root.config(menu=self.toplevel_menu)

    def include_folder(self):
        logconf: LogConfiguration = LogConfiguration()

        if logconf.include_folder(fd.askdirectory(parent=root, title="Include a folder")):
            print(f"log config: {logconf}")
            self.toplevel_menu.event_generate(self.RefreshLogConfigEvent)


    def exclude_folder(self):
        logconf: LogConfiguration = LogConfiguration()

        if logconf.exclude_folder(fd.askdirectory(parent=root, title="Exclude a folder")):
            print(f"log config: {logconf}")
            self.toplevel_menu.event_generate(self.RefreshLogConfigEvent)

class LogFileInfo:
    class LogLineInfo:
        def __init__(self, line: str, color: str) -> None:
            self.color: str = color
            self.valid: bool = False

            match = re.compile(
                r"\[(?P<level>\w+)]\s"
                r"\((?P<address>[^)]+)\)\s"
                r"(?P<time>\d{4}-\d{2}-\d{2} \d{2}:\d{2}:\d{2})\s-\s"
                r"(?P<message>.+)"
            ).match(line)
            if not match:
                return

            self.level = match.group("level")
            self.address = match.group("address")
            self.timestamp = datetime.strptime(match.group("time"), "%Y-%m-%d %H:%M:%S")
            self.message = match.group("message").replace("\u0001", "|")
            self.valid =  True

        def __str__(self):
            data: Dict[str, str] = {
                "Level": self.level,
                "Address": self.address,
                "Time": str(self.timestamp),
                "Message": self.message
            }
            return json.dumps(data, indent=2)


    def __init__(self, path: str, color: str):
        self.color = color
        self.path = path
        self.lines: List[LogFileInfo.LogLineInfo] = []

        print(f"Generating log info for file: {self.path }")
        with open(self.path) as file:
            for line in file:
                line_info: LogFileInfo.LogLineInfo = LogFileInfo.LogLineInfo(line.rstrip(), color)
                if line_info.valid:
                    print(f"Line data: {line_info}")
                    self.lines.append(line_info)

class LogViewer:
    def __init__(self, root: tk.Frame) -> None:
        self.frame: tk.Frame = tk.Frame(root)
        self.frame.pack(side=tk.RIGHT, fill=tk.Y)

        scrollbar = tk.Scrollbar(self.frame)
        scrollbar.pack(side=tk.RIGHT, fill=tk.Y)

        self.log_text = tk.Text(self.frame, yscrollcommand=scrollbar.set)
        self.log_text.pack(side=tk.LEFT, fill=tk.BOTH, expand=True)

        scrollbar.config(command=self.log_text.yview)

    def refresh(self, tracked_file: List[str]) -> None:
        self.log_text.config(state="normal")
        self.log_text.delete("1.0", tk.END)
        logfile_info: List[LogFileInfo] = []

        color_set: Set[str] = self._generate_setcolor(len(tracked_file))
        for file, color in zip(tracked_file, color_set):
            logfile_info.append(LogFileInfo(file, color))

        log_list: List[LogFileInfo.LogLineInfo] = []
        for index, logfile_info in enumerate(logfile_info):
            log_list.extend(logfile_info.lines)

        log_list.sort(key=lambda log: log.timestamp)

        for index, line in enumerate(log_list):
            tag_name: str = f"addr_{index}"
            log_text: str = f"[{line.timestamp.strftime('%Y-%m-%d %H:%M:%S')}] - [{line.level}] ({line.address}) {line.message}\n"

            self.log_text.insert(tk.END, log_text)
            self.log_text.tag_add(tag_name, f"{index}.0", f"{index}.end")
            self.log_text.tag_config(tag_name, background=line.color)

        self.log_text.config(state="disabled")

    @staticmethod
    def _generate_setcolor(n: int) -> Set[str]:
        color_set: Set[str] = set()

        while len(color_set) != n:
            color_set.add("#"+("%06x" % random.randint(0, 255 ** 3)))
        return color_set

class LogInfoBar:
    LogRefreshDisplayEvent: str = "<<log-refresh-display>>"
    LogRefreshFileEvent: str = "<<log-refresh-file>>"

    def __init__(self, root: tk.Frame):
        self.frame = tk.Frame(root, width=300)
        self.frame.pack(side=tk.LEFT, fill=tk.Y)
        self.frame.pack_propagate(False)

        self.tree = ttk.Treeview(self.frame)
        self.tree.pack(fill=tk.BOTH, expand=True, padx=2, pady=2)

        self.refresh = tk.Button(self.frame, text="Refresh", command=self.onRefreshClick)
        self.refresh.pack(fill=tk.BOTH, expand=True)

    def onRefreshClick(self):
        print("Refresh requested")
        self.frame.event_generate(self.LogRefreshDisplayEvent)

class LogContainer:
    def __init__(self, root: tk.Tk):
        self.frame: tk.Frame = tk.Frame(root)
        self.frame.pack(fill=tk.BOTH, expand=True)

        self.viewer: LogViewer = LogViewer(self.frame)
        self.viewbar: LogInfoBar = LogInfoBar(self.frame)

        self.tracked_file: Set[str] = set()
        self.refreshLog(None)

        self.viewbar.frame.bind(LogInfoBar.LogRefreshDisplayEvent, self.refreshDisplay)
        self.viewbar.frame.bind(LogInfoBar.LogRefreshFileEvent, self.refreshLog)

    def refreshLog(self, _) -> None:
        logconf: LogConfiguration = LogConfiguration()

        for folder in logconf.inc_folder:
            for root, _, files in os.walk(folder):
                if any(os.path.commonpath([root, exc]) == exc for exc in logconf.exc_folder):
                    continue
                for f in files:
                    full_path = os.path.abspath(os.path.join(root, f))
                    if full_path not in logconf.exc_file:
                        self.tracked_file.add(full_path)
        for file in logconf.inc_file:
            full_path = os.path.abspath(file)
            if full_path not in logconf.exc_file:
                self.tracked_file.add(full_path)

        print("--------------------")
        print(f"Tracked file: {self.tracked_file}")
        print("--------------------")
        self.viewbar.frame.event_generate(LogInfoBar.LogRefreshDisplayEvent)

    def refreshDisplay(self, _) -> None:
        self.viewer.refresh(self.tracked_file)

if __name__ == "__main__":
    root = tk.Tk()

    menu: TopLevelMenu = TopLevelMenu(root)
    logcontainer: LogContainer = LogContainer(root)

    menu.toplevel_menu.bind(TopLevelMenu.RefreshLogConfigEvent, logcontainer.refreshLog)

    root.mainloop()