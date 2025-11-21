![QDiffX Demo](assets/qdiffx_demo.svg)
# QDiffX
## The Diff Widget Qt Should Have Built


**QDiffX** is a plug-and-play **diff visualization widget for Qt applications**, designed to make content comparison effortless and native. Whether you're building a file manager, a version control GUI, a config editor, or an IDE QDiffX brings a clean, intuitive diff viewer directly into your Qt interface, no web views or external dependencies required.

**Finally.** A production-ready, drop-in diff visualization widget for Qt applications. Because handling file conflicts shouldn't require reinventing the wheel.


## Why QDiffX Changes Everything

**Qt developers have been waiting for this.** Every file manager, code editor, version control GUI, and configuration tool needs diff visualization. Until now, you had to build it from scratch or compromise with web-based solutions.

**QDiffX ends that pain.**

---

## Perfect For

* **📁 File Managers**  Show users exactly what changed when files conflict
* **⚙️ Configuration Tools**  Let users merge settings intelligently
* **🔧 Development IDEs** Built-in diff without external dependencies
* **📊 Database GUIs** (Redis, MongoDB tools) - Compare configurations
* **🏢 Business Tools** Solid diff widget in your Qt stack

---

##  What Makes It Unstoppable

```cpp
// Literally this simple
#include "QDiffWidget.h"

QDiffX::QDiffWidget* diff = new QDiffX::QDiffWidget();
diff->setLeftContent(originalFile);
diff->setRightContent(modifiedFile);
layout->addWidget(diff);
// Done. Ready to use.
```
Built with a pluggable algorithm architecture: write a bridge for your algorithm, register it, and integrate directly.
---


##  The Problem It Solves

Every Qt developer has faced this:

* ❌ User opens a file that changed on disk
* ❌ Application needs to show differences
* ❌ No built-in Qt solution exists
* ❌ Weeks spent building custom diff logic
* ❌ Inconsistent UX across applications
* ❌ Integration of heavy diffing algorithms like myers can be overkill sometimes and takes huge time and effort

**QDiffX eliminates this entirely.**


## 🛠️ Installation

### Quick Start

```bash
git clone https://github.com/yourusername/QDiffX.git
cmake -S QDiffX -B QDiffX/build
cmake --build QDiffX/build
# Run the demo app from the build output
```

### CMake Integration (embed the widget)

```cmake
# Add QDiffX as a subproject
add_subdirectory(QDiffX)

# Add widget sources to your target and link core library
target_sources(your_target PRIVATE
    QDiffX/src/QDiffWidget.cpp
    QDiffX/src/QDiffTextBrowser.cpp
    QDiffX/src/QLineNumberArea.cpp
)
target_include_directories(your_target PRIVATE ${CMAKE_SOURCE_DIR}/QDiffX/src)
target_link_libraries(your_target PRIVATE
    QDiffXCore
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Widgets
)
```

### qmake Integration

Not supported yet (planned).

---

##  Use Cases That Work

1. **File Sync Apps**  When Dropbox meets Qt
2. **Configuration Managers**  Docker Compose, Kubernetes configs
3. **Database Tools**  Redis config comparisons, SQL migrations
4. **Game Development**  Asset version control, save file debugging
5. **IoT Dashboards**  Device configuration drift detection

### Basic Usage

```cpp
#include "QDiffWidget.h"

// Create the widget
auto* diffWidget = new QDiffX::QDiffWidget(this);

// Set content to compare
diffWidget->setLeftContent("Original content\nLine 2\nLine 3");
diffWidget->setRightContent("Modified content\nLine 2 changed\nLine 3");

// Add to your layout
layout->addWidget(diffWidget);
```

### Display Modes

```cpp
// Side-by-side (default)
diffWidget->setDisplayMode(QDiffX::QDiffWidget::DisplayMode::SideBySide);

// Inline
diffWidget->setDisplayMode(QDiffX::QDiffWidget::DisplayMode::Inline);
```

### Load From Files

```cpp
diffWidget->setContentFromFiles("/path/to/left.txt", "/path/to/right.txt");
```

### Algorithm Management

```cpp
#include "QAlgorithmRegistry.h"
#include "QAlgorithmManager.h"
#include "DMPAlgorithm.h"

// Register algorithm
QDiffX::QAlgorithmRegistry::get_Instance().registerAlgorithm<QDiffX::DMPAlgorithm>("dmp");

// Configure manager
auto* manager = new QDiffX::QAlgorithmManager(diffWidget);
manager->setSelectionMode(QDiffX::QAlgorithmSelectionMode::Auto);
manager->setExecutionMode(QDiffX::QExecutionMode::Synchronous);
diffWidget->setAlgorithmManager(manager);

// Or manual selection
manager->setSelectionMode(QDiffX::QAlgorithmSelectionMode::Manual);
manager->setCurrentAlgorithm("dmp");
```

### Supported Versions

- Qt 5.15+ and Qt 6.x
- C++17

## Contributing

Join the movement and help make QDiffX even better:

*  **Bug Reports**  Help us make it bulletproof
*  **Feature Requests**  What's your dream diff widget?
*  **Code Contributions**  Let's build the future of Qt together
*  **Documentation**  Help others discover the power

- Pull requests automatically trigger cross‑platform builds and tests on Ubuntu, Windows, and macOS.
- CI installs a compatible Qt version, configures CMake with `BUILD_TESTING=ON`, builds, and runs `ctest`.
- Failing tests and build errors block merges to protected branches.
- Static analysis (`cppcheck`, `clang-tidy`) runs on Linux and reports are uploaded as artifacts.
- Release tags (`v*`) package build outputs per OS and publish them to GitHub Releases.
- See workflow configuration in `.github/workflows/build.yml`.


### How to Contribute

1. Fork the repository
2. Create a feature branch (`git checkout -b feature/X-feature`)
3. Make your changes
4. Commit your changes (`git commit -m 'Add X feature'`)
5. Push to the branch (`git push origin feature/X-feature`)
6. Open a Pull Request



### Upcoming Features
ideas that will be made in the future

- [ ] **Inline Editing**  Edit differences directly in the widget
- [ ] **Directory Comparison**  Compare entire folder structures
- [ ] **Advanced Merge Tools**  Three-way merge support
- [ ] **More themes**  
- [ ] **Better UI with customizable buttons**  to handle different algorithms choice and inline / side-by-side directly from the widget 
- [ ] **Smart algorithm choice learning system**  can be trained as it is used to always pick the best possible algorithm depending on the file size and structure 


## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

### Testing

```bash
cmake -S . -B build
cmake --build build
ctest --test-dir build --output-on-failure
```






**⭐ Star this repository if QDiffX solves your Qt diff headaches!**
