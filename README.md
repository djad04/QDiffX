![QDiffX Demo](assets/qdiffx_demo.svg)

### The Diff Widget Qt Should Have Built

**QDiffX** is a lightweight, extensible **diff viewer widget for Qt**.  
It is designed to be dropped into existing Qt applications without web views, heavy dependencies, or complex setup.

The project focuses on two core ideas:
- A **clean, usable diff UI** (inline and side-by-side)
- A **pluggable algorithm system** so diff logic stays flexible and replaceable
- **Synchronous and asynchronous execution** for both responsive UIs and immediate results

---

## What It Is

- A ready-to-use **Qt Widgets diff component**
- Supports **side-by-side** and **inline** diff views
- Built with **Qt Widgets** (Qt 5.15+ and Qt 6.x)
- Written in **C++17**
- Can be embedded or extended easily

This is meant to be infrastructure something you don't have to rewrite for every Qt project.

---

## Typical Use Cases

- File managers and sync tools  
- IDEs and code editors  
- Configuration and settings comparison  
- Database and DevOps tools  
- Any Qt application that needs to show "what changed"

---

## Quick Example
```cpp
#include "QDiffWidget.h"

auto* diff = new QDiffX::QDiffWidget(this);
diff->setLeftContent(originalText);
diff->setRightContent(modifiedText);
layout->addWidget(diff);
```

That's it the widget handles rendering, scrolling, and highlighting.

---

## Display Modes
```cpp
// Side-by-side (default)
diff->setDisplayMode(QDiffX::QDiffWidget::SideBySide);

// Inline
diff->setDisplayMode(QDiffX::QDiffWidget::Inline);
```

---

## Algorithm Plugin System

QDiffX cleanly separates diff visualization from diff algorithms.

Algorithms are implemented as small, self-contained classes and registered with the system.

### Why This Matters

- Swap algorithms without touching UI code
- Use simple algorithms for small files
- Plug in advanced algorithms for large or structured content
- Easy to experiment or customize behavior

### Adding a New Algorithm

1. Create a class that inherits from `QDiffX::QDiffAlgorithm`
2. Implement:
   - `calculateDiff(...)`
   - `getName()`
   - `getDescription()`
3. Register it with the algorithm manager or registry
```cpp
registry.registerAlgorithm(
    std::make_shared<MyCustomDiffAlgorithm>()
);
```

Once registered, the algorithm appears in the UI automatically and can be selected at runtime.

---

## Algorithm Management
```cpp
auto* manager = new QDiffX::QAlgorithmManager(diff);
manager->setSelectionMode(QDiffX::QAlgorithmSelectionMode::Auto);
diff->setAlgorithmManager(manager);
```

Manual selection is also supported if a specific algorithm is required.

---

### Algorithm Capabilities

Each algorithm declares its capabilities:

- Support for large files
- Unicode and binary content support
- Line-by-line, character-by-character, or word-by-word diffing
- Maximum recommended file size
- Performance complexity estimation

The algorithm manager uses these capabilities for intelligent automatic selection.

---

## Execution Modes

QDiffX supports both synchronous and asynchronous diff calculation.

### Asynchronous Execution (Recommended)
```cpp
manager->setExecutionMode(QDiffX::QExecutionMode::Asynchronous);

auto future = manager->calculateDiffAsync(leftText, rightText);
// UI remains responsive while calculation runs in background
```

Connect to signals to receive results:
```cpp
connect(manager, &QDiffX::QAlgorithmManager::diffCalculated,
        this, [](const QDiffX::QDiffResult& result) {
    if (result.success()) {
        // Use the result
    }
});
```

### Synchronous Execution
```cpp
manager->setExecutionMode(QDiffX::QExecutionMode::Synchronous);

auto result = manager->calculateDiffSync(leftText, rightText);
if (result.success()) {
    // Process immediately
}
```

---

## Error Handling

QDiffX provides comprehensive error handling through typed error codes and detailed error messages.

### Error Types
```cpp
enum class QAlgorithmManagerError {
    None,                      // No error
    AlgorithmNotFound,         // Requested algorithm doesn't exist
    AlgorithmCreationFailed,   // Failed to instantiate algorithm
    InvalidAlgorithmId,        // Invalid algorithm identifier
    DiffExecutionFailed,       // Algorithm execution failed
    ConfigurationError,        // Invalid algorithm configuration
    Timeout,                   // Operation exceeded time limit
    OperationCancelled,        // User cancelled operation
    Unknown                    // Unexpected error
};
```

### Handling Errors
```cpp
auto result = manager->calculateDiffSync(leftText, rightText);

if (!result.success()) {
    QString error = result.errorMessage();
    qWarning() << "Diff failed:" << error;
    
    // Check specific error type
    auto errorType = manager->lastError();
    if (errorType == QDiffX::QAlgorithmManagerError::AlgorithmNotFound) {
        // Handle missing algorithm
    }
}
```

### Error Signals
```cpp
connect(manager, &QDiffX::QAlgorithmManager::errorOccurred,
        this, [](QDiffX::QAlgorithmManagerError error, const QString& message) {
    qCritical() << "Algorithm error:" << message;
});
```

### Fallback Algorithm

Configure a fallback algorithm for automatic recovery:
```cpp
manager->setCurrentAlgorithm("advanced-algorithm");
manager->setFallBackAlgorithm("simple-algorithm");

// If advanced-algorithm fails, simple-algorithm is automatically tried
```

---

## Thread Safety

QDiffX is designed with thread safety in mind:

- Algorithm registry uses `QMutex` for concurrent access
- Algorithm manager supports asynchronous execution via `QFuture`
- Safe to call from multiple threads when using the registry
- UI updates are handled on the main thread

---

## Algorithm Configuration

Algorithms can be configured at runtime:
```cpp
// Get current configuration
auto config = manager->getAlgorithmConfiguration("dmp");

// Modify settings
config["timeout"] = 5000;
config["checklines"] = true;

// Apply new configuration
manager->setAlgorithmConfiguration("dmp", config);
```

Configuration changes emit signals for reactive updates:
```cpp
connect(manager, &QDiffX::QAlgorithmManager::algorithmConfigurationChanged,
        this, [](const QString& algorithmId, const QMap<QString, QVariant>& config) {
    qDebug() << "Algorithm" << algorithmId << "reconfigured";
});
```

---

## Side-by-Side Diff Results

QDiffX provides specialized support for side-by-side diffs:
```cpp
auto sideBySideResult = manager->calculateSideBySideDiffSync(leftText, rightText);

if (sideBySideResult.success()) {
    // Left side contains Equal + Delete operations
    auto leftChanges = sideBySideResult.leftSide.changes();
    
    // Right side contains Equal + Insert operations
    auto rightChanges = sideBySideResult.rightSide.changes();
    
    // Algorithm used for calculation
    QString algorithm = sideBySideResult.algorithmUsed;
}
```

---

## Metadata and Performance

Diff results include metadata for analysis:
```cpp
auto result = manager->calculateDiffSync(leftText, rightText);

// Access metadata
auto metadata = result.allMetaData();
int executionTime = metadata["executionTime"].toInt();
QString algorithmUsed = metadata["algorithm"].toString();
int changeCount = metadata["changeCount"].toInt();
```
---

## Building
```bash
git clone https://github.com/yourusername/QDiffX.git
cmake -S QDiffX -B build
cmake --build build
```

A demo application is included to test the widget and available algorithms.

---

## Contributing

Contributions are welcome and should be small and focused.

**Key points:**
- Unit tests are required for new algorithms and logic changes
- Tests run automatically in CI
- Builds and tests run on Linux, Windows, and macOS
- Failing tests block merges to protected branches

**If you add:**
- A new algorithm → include tests
- A bug fix → include a regression test

This keeps the core stable and predictable.

---

## Roadmap

- Add More themes
- Direct editing
- Directory comparison
- Three-way merge
- Additional themes
- Smarter automatic algorithm selection

---

## License

MIT License — see the [LICENSE](LICENSE) file for details.
