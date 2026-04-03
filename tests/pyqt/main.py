import sys
import os
from PyQt6.QtWidgets import QApplication, QWidget, QVBoxLayout, QLabel
from PyQt6.QtCore import QTimer, Qt
from PyQt6.QtGui import QImage, QPixmap
import pybindcef

class PyQtCefTest(QWidget):
    def __init__(self):
        super().__init__()
        self.setWindowTitle("PyQt6 CEF Test")
        self.resize(800, 600)

        res_path = os.path.abspath("./Resources")
        sub_path = os.path.abspath("./cef_worker")
        pybindcef.initialize(sub_path, res_path)

        self.layout = QVBoxLayout(self)

        self.layout.setContentsMargins(0, 0, 0, 0)
        self.layout.setSpacing(0)

        self.view = QLabel("Loading...")
        self.view.setMinimumSize(1, 1) 
        self.layout.addWidget(self.view)

        pybindcef.create_browser("https://github.com", self.on_paint)

        self.timer = QTimer()
        self.timer.timeout.connect(pybindcef.do_work)
        self.timer.start(0)

    def on_paint(self, buffer, width, height):
        img = QImage(buffer, width, height, QImage.Format.Format_ARGB32)
        safe_img = img.copy() 
        
        self.view.setPixmap(QPixmap.fromImage(safe_img))

    def resizeEvent(self, event):
        super().resizeEvent(event)
        size = event.size()
        w, h = size.width(), size.height()
        if w > 0 and h > 0:
            pybindcef.resize(w, h)

    def closeEvent(self, event):
        pybindcef.shutdown()
        event.accept()

if __name__ == "__main__":
    app = QApplication(sys.argv)
    window = PyQtCefTest()
    window.show()
    sys.exit(app.exec())
