# 🎨 ASCII Art Generator (C++ + OpenCV)

This project converts any image into **colored ASCII art** directly viewable in your terminal.  
It uses OpenCV for image processing and ANSI escape codes to colorize each ASCII character based on the original pixel colors.

---

## 🚀 Features
- 🖼 Converts any image into ASCII characters
- 🌈 Supports **true RGB colors** in the terminal
- ⚙️ Automatically scales the image to fit your console
- 🧮 Brightness-based ASCII mapping for accurate results
- ⌨️ Pass the image on the command line — no code editing or rebuilding needed
- 🧠 Easy to customize: you can change the ASCII ramp, size, or scaling

---

## 🧰 Requirements

Make sure you have:
- **C++17 or later**
- **OpenCV 4.x** installed (the repo includes the OpenCV 4.12 headers and `.lib` files)
- The **OpenCV runtime DLL** (`opencv_world4120.dll`) available at runtime — see [Runtime DLL](#-runtime-dll)
- A terminal that supports **ANSI 24-bit colors**

---

## 🛠️ Installation

### 1. Clone the repository:
```bash
git clone https://github.com/YOUR_USERNAME/ascii-art-generator.git
cd ascii-art-generator
```

### 2. Build the project:

**Visual Studio (this repository):**
```bash
msbuild LearningOpenCV.sln /p:Configuration=Release /p:Platform=x64
```
Or open `LearningOpenCV.sln` in Visual Studio and build **Release | x64**.

The executable ends up in `x64\Release\LearningOpenCV.exe`.

**With g++:**
```bash
g++ -std=c++17 main.cpp -o ascii -lopencv_core -lopencv_imgproc -lopencv_imgcodecs -lopencv_highgui
```

**With CMake**, create a `CMakeLists.txt`:
```cmake
cmake_minimum_required(VERSION 3.10)
project(AsciiArt)
find_package(OpenCV REQUIRED)
add_executable(AsciiArt main.cpp)
target_link_libraries(AsciiArt ${OpenCV_LIBS})
```
Then:
```bash
mkdir build && cd build
cmake ..
make
```

---

## 🖼️ Usage

Pass the image path as the only argument:

```bash
run.cmd Resources\ronaldoAndMessi.jpg
run.cmd "C:\Users\me\Pictures\my photo.png"
```

`run.cmd` is a small wrapper at the repo root that:
- works from any directory,
- builds **Release x64** once with MSBuild if no executable exists yet,
- lets you **drag an image file onto `run.cmd`** in Explorer,
- forwards your arguments and the program's exit code.

You can also call the executable directly:

```bash
x64\Release\LearningOpenCV.exe path\to\image.jpg
```

Other commands:

```bash
run.cmd --help                         # usage and exit codes
run.cmd path\to\image.jpg > art.txt    # save the art (colors included) to a file
```

### Exit codes

| Code | Meaning |
| ---- | ------- |
| 0 | Success |
| 1 | The image could not be loaded |
| 2 | Invalid command line (missing image or unknown option) |

Paths containing spaces must be quoted. Both `/` and `\` work as separators.

---

## 📦 Runtime DLL

OpenCV is linked dynamically, so the program needs `opencv_world4120.dll` (Release) or `opencv_world4120d.dll` (Debug) when it starts. The repo only contains the import libraries, not the DLL. Put it in either place:

- `Dependencies\OpenCV\bin\` — `run.cmd` adds this folder to `PATH` automatically, or
- next to `LearningOpenCV.exe`, or
- anywhere on your system `PATH`.

If the DLL is missing, `run.cmd` reports it instead of failing silently.

---

## ⚙️ Customization

The defaults live at the top of `LearningOpenCV/main.cpp`:
```cpp
constexpr char kAsciiRamp[] = " .`'^\",:;Il!i~+_-?][}{1)(|\\/tfjrxnuvczXYUJCLQ0OZmwqpdbkhao*#MW&8%B@$@█▓▒░";
constexpr int kConsoleWidth = 220;  // characters per line
constexpr double kCharAspect = 1.0; // tweak if the output looks stretched
```
Change them there for now; they are the intended hook for future `--width`, `--chars`, and `--aspect` flags.

---

## 🧑‍💻 Example Output
<img width="474" height="813" alt="Screenshot 2025-10-10 162439" src="https://github.com/user-attachments/assets/fe787da5-2a51-464c-a046-3e24caaf74d8" />



## 🧩 Future Improvements
- Add **auto-detection** for terminal width/height
- Add **grayscale preview** mode
- Add **video-to-ASCII** conversion
- Add flags for width, character ramp, aspect ratio, and saving to a file

---

## 📜 License
This project is open-source under the **MIT License**.

---

## 💡 Author
**Saad Rabia**  
Passionate about C++, graphics, and visual computing.
