#include <QObject>
#include <QtTest/QtTest>
#include "../src/QAlgorithmManager.h"
#include "../src/QAlgorithmRegistry.h"
#include "../src/DMPAlgorithm.h"

class Tst_QAlgorithmManager : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();
    void testDefaultAlgorithmsRegistered();
    void testAlgorithmRegistration_data();
    void testAlgorithmRegistration();
    void testUnregisterAlgorithm();
    void testCreateAlgorithm();
    void testGetAvailableAlgorithms();
    void testGetAlgorithmInfo();
    void testIsAlgorithmAvailable();
    void testRegistryClear();
    void testSetAndGetAlgorithmConfiguration();
    void testGetAlgorithmConfigurationKeys();
    void testErrorHandling_Registry();
    void testCalculateDiffSync_AutoSelection_data();
    void testCalculateDiffSync_AutoSelection();
    void testCalculateDiffSync_ManualSelection();
    void testCalculateDiffAsync_AutoSelection();
    void testCalculateDiffAsync_ManualSelection();
    void testIsAlgorithmAvailable_Manager();
    void testSelectionMode();
    void testExecutionMode();
    void testCurrentAlgorithm();
    void testFallBackAlgorithm();
    void testErrorOutputEnabled();
    void testResetManager();
    void testErrorHandling_Manager();
    void testSignals();
};

void Tst_QAlgorithmManager::initTestCase() {}
void Tst_QAlgorithmManager::cleanupTestCase() { QDiffX::QAlgorithmRegistry::get_Instance().clear(); }

void Tst_QAlgorithmManager::testDefaultAlgorithmsRegistered() {
    QDiffX::QAlgorithmRegistry& registry = QDiffX::QAlgorithmRegistry::get_Instance();
    QVERIFY(registry.isAlgorithmAvailable("dmp"));
    QVERIFY(registry.isAlgorithmAvailable("dtl"));
    QVERIFY(registry.getAvailableAlgorithms().count() >= 2);
}

void Tst_QAlgorithmManager::testAlgorithmRegistration_data() {
    QTest::addColumn<QString>("algorithmId");
    QTest::addColumn<bool>("expectedResult");
    QTest::newRow("valid registration") << "NewAlgorithm" << true;
    QTest::newRow("already registered") << "dmp" << false;
    QTest::newRow("empty ID") << "" << false;
}

void Tst_QAlgorithmManager::testAlgorithmRegistration() {
    QFETCH(QString, algorithmId);
    QFETCH(bool, expectedResult);
    QDiffX::QAlgorithmRegistry::get_Instance().clear();
    if (algorithmId == "dmp" && !expectedResult) {
        QDiffX::QAlgorithmRegistry::get_Instance().registerAlgorithm<QDiffX::DMPAlgorithm>("dmp");
    }
    bool result = QDiffX::QAlgorithmRegistry::get_Instance().registerAlgorithm<QDiffX::DMPAlgorithm>(algorithmId);
    QCOMPARE(result, expectedResult);
    if (expectedResult) {
        QVERIFY(QDiffX::QAlgorithmRegistry::get_Instance().isAlgorithmAvailable(algorithmId));
    } else {
        if (algorithmId != "dmp") {
            QVERIFY(!QDiffX::QAlgorithmRegistry::get_Instance().isAlgorithmAvailable(algorithmId));
        }
    }
}

void Tst_QAlgorithmManager::testUnregisterAlgorithm() {
    QDiffX::QAlgorithmRegistry& registry = QDiffX::QAlgorithmRegistry::get_Instance();
    registry.clear();
    registry.registerAlgorithm<QDiffX::DMPAlgorithm>("TempAlgorithm");
    QVERIFY(registry.isAlgorithmAvailable("TempAlgorithm"));
    QVERIFY(registry.unregisterAlgorithm("TempAlgorithm"));
    QVERIFY(!registry.isAlgorithmAvailable("TempAlgorithm"));
    QVERIFY(!registry.unregisterAlgorithm("NonExistentAlgorithm"));
}

void Tst_QAlgorithmManager::testCreateAlgorithm() {
    QDiffX::QAlgorithmRegistry& registry = QDiffX::QAlgorithmRegistry::get_Instance();
    registry.clear();
    registry.registerAlgorithm<QDiffX::DMPAlgorithm>("DMPAlgorithm");
    std::unique_ptr<QDiffX::QDiffAlgorithm> algo = registry.createAlgorithm("DMPAlgorithm");
    QVERIFY(algo != nullptr);
    QCOMPARE(algo->getName(), QDiffX::DMPAlgorithm().getName());
    std::unique_ptr<QDiffX::QDiffAlgorithm> nonExistentAlgo = registry.createAlgorithm("NonExistent");
    QVERIFY(nonExistentAlgo == nullptr);
}

void Tst_QAlgorithmManager::testGetAvailableAlgorithms() {
    QDiffX::QAlgorithmRegistry& registry = QDiffX::QAlgorithmRegistry::get_Instance();
    registry.clear();
    registry.registerAlgorithm<QDiffX::DMPAlgorithm>("Algo1");
    registry.registerAlgorithm<QDiffX::DMPAlgorithm>("Algo2");
    QStringList algorithms = registry.getAvailableAlgorithms();
    QVERIFY(algorithms.count() >= 2);
    QVERIFY(algorithms.contains("dmp"));
    QVERIFY(algorithms.contains("dtl"));
    QVERIFY(algorithms.contains("Algo1"));
    QVERIFY(algorithms.contains("Algo2"));
}

void Tst_QAlgorithmManager::testGetAlgorithmInfo() {
    QDiffX::QAlgorithmRegistry& registry = QDiffX::QAlgorithmRegistry::get_Instance();
    registry.clear();
    registry.registerAlgorithm<QDiffX::DMPAlgorithm>("TestAlgo");
    std::optional<QDiffX::QAlgorithmInfo> info = registry.getAlgorithmInfo("TestAlgo");
    QVERIFY(info.has_value());
    QCOMPARE(info->name, QDiffX::DMPAlgorithm().getName());
    std::optional<QDiffX::QAlgorithmInfo> nonExistentInfo = registry.getAlgorithmInfo("NonExistent");
    QVERIFY(!nonExistentInfo.has_value());
}

void Tst_QAlgorithmManager::testIsAlgorithmAvailable() {
    QDiffX::QAlgorithmRegistry& registry = QDiffX::QAlgorithmRegistry::get_Instance();
    registry.clear();
    registry.registerAlgorithm<QDiffX::DMPAlgorithm>("TestAlgo");
    QVERIFY(registry.isAlgorithmAvailable("TestAlgo"));
    QVERIFY(!registry.isAlgorithmAvailable("NonExistent"));
}

void Tst_QAlgorithmManager::testRegistryClear() {
    QDiffX::QAlgorithmRegistry& registry = QDiffX::QAlgorithmRegistry::get_Instance();
    registry.registerAlgorithm<QDiffX::DMPAlgorithm>("AlgoToClear");
    QVERIFY(registry.isAlgorithmAvailable("AlgoToClear"));
    registry.clear();
    QVERIFY(!registry.isAlgorithmAvailable("AlgoToClear"));
    QVERIFY(registry.isAlgorithmAvailable("dmp"));
    QVERIFY(registry.isAlgorithmAvailable("dtl"));
}

void Tst_QAlgorithmManager::testSetAndGetAlgorithmConfiguration() {
    QDiffX::QAlgorithmRegistry& registry = QDiffX::QAlgorithmRegistry::get_Instance();
    registry.clear();
    registry.registerAlgorithm<QDiffX::DMPAlgorithm>("ConfigurableAlgo");
    QMap<QString, QVariant> config;
    config["precision"] = 0.5;
    config["timeout"] = 1000;
    QVERIFY(registry.setAlgorithmConfiguration("ConfigurableAlgo", config));
    QMap<QString, QVariant> retrievedConfig = registry.getAlgorithmConfiguration("ConfigurableAlgo");
    QCOMPARE(retrievedConfig.value("precision").toDouble(), 0.5);
    QCOMPARE(retrievedConfig.value("timeout").toInt(), 1000);
    QVERIFY(!registry.setAlgorithmConfiguration("NonExistentAlgo", config));
    QCOMPARE(registry.getAlgorithmConfiguration("NonExistentAlgo").count(), 0);
}

void Tst_QAlgorithmManager::testGetAlgorithmConfigurationKeys() {
    QDiffX::QAlgorithmRegistry& registry = QDiffX::QAlgorithmRegistry::get_Instance();
    registry.clear();
    registry.registerAlgorithm<QDiffX::DMPAlgorithm>("ConfigurableAlgo");
    QMap<QString, QVariant> config;
    config["param1"] = 1;
    config["param2"] = "value";
    registry.setAlgorithmConfiguration("ConfigurableAlgo", config);
    QStringList keys = registry.getAlgorithmConfigurationKeys("ConfigurableAlgo");
    QStringList expectedKeys = QDiffX::DMPAlgorithm().getConfigurationKeys();
    for (const QString& key : expectedKeys) {
        QVERIFY(keys.contains(key));
    }
    QCOMPARE(registry.getAlgorithmConfigurationKeys("NonExistentAlgo").count(), 0);
}

void Tst_QAlgorithmManager::testErrorHandling_Registry() {
    QDiffX::QAlgorithmRegistry& registry = QDiffX::QAlgorithmRegistry::get_Instance();
    registry.clear();
    registry.setErrorOutputEnabled(true);
    registry.registerAlgorithm<QDiffX::DMPAlgorithm>("");
    QCOMPARE(registry.lastError(), QDiffX::QAlgorithmRegistryError::EmptyAlgorithmId);
    registry.createAlgorithm("NonExistent");
    QCOMPARE(registry.lastError(), QDiffX::QAlgorithmRegistryError::AlgorithmNotFound);
    registry.unregisterAlgorithm("NonExistent");
    QCOMPARE(registry.lastError(), QDiffX::QAlgorithmRegistryError::AlgorithmNotFound);
    registry.setErrorOutputEnabled(false);
}

void Tst_QAlgorithmManager::testCalculateDiffSync_AutoSelection_data() {
    QTest::addColumn<QString>("left");
    QTest::addColumn<QString>("right");
    QTest::addColumn<int>("expectedDiffOperations");
    QTest::newRow("identical strings") << "hello" << "hello" << 1;
    QTest::newRow("different strings") << "hello" << "world" << 2;
    QTest::newRow("empty strings") << "" << "" << 0;
    QTest::newRow("left empty") << "" << "world" << 1;
    QTest::newRow("right empty") << "hello" << "" << 1;
}

void Tst_QAlgorithmManager::testCalculateDiffSync_AutoSelection() {
    QFETCH(QString, left);
    QFETCH(QString, right);
    QDiffX::QAlgorithmRegistry::get_Instance().clear();
    QDiffX::QAlgorithmRegistry::get_Instance().registerAlgorithm<QDiffX::DMPAlgorithm>("dmp");
    QDiffX::QAlgorithmManager manager;
    manager.setSelectionMode(QDiffX::QAlgorithmSelectionMode::Auto);
    manager.setExecutionMode(QDiffX::QExecutionMode::Synchronous);
    manager.setErrorOutputEnabled(true);
    QDiffX::QDiffResult result = manager.calculateDiffSync(left, right);
    QVERIFY(result.success());
    QCOMPARE(manager.lastError(), QDiffX::QAlgorithmManagerError::None);
}

void Tst_QAlgorithmManager::testCalculateDiffSync_ManualSelection() {
    QDiffX::QAlgorithmRegistry::get_Instance().clear();
    QDiffX::QAlgorithmRegistry::get_Instance().registerAlgorithm<QDiffX::DMPAlgorithm>("DMPAlgorithm");
    QDiffX::QAlgorithmManager manager;
    manager.setSelectionMode(QDiffX::QAlgorithmSelectionMode::Manual);
    manager.setExecutionMode(QDiffX::QExecutionMode::Synchronous);
    manager.setCurrentAlgorithm("DMPAlgorithm");
    manager.setErrorOutputEnabled(true);
    QDiffX::QDiffResult result = manager.calculateDiffSync("test1", "test2", QDiffX::QAlgorithmSelectionMode::Manual, "DMPAlgorithm");
    QVERIFY(result.success());
    QCOMPARE(manager.lastError(), QDiffX::QAlgorithmManagerError::None);
    QDiffX::QDiffResult invalidResult = manager.calculateDiffSync("test1", "test2", QDiffX::QAlgorithmSelectionMode::Manual, "NonExistentAlgorithm");
    QVERIFY(!invalidResult.success());
    QCOMPARE(manager.lastError(), QDiffX::QAlgorithmManagerError::AlgorithmNotFound);
}

void Tst_QAlgorithmManager::testCalculateDiffAsync_AutoSelection() {
    QDiffX::QAlgorithmRegistry::get_Instance().clear();
    QDiffX::QAlgorithmRegistry::get_Instance().registerAlgorithm<QDiffX::DMPAlgorithm>("dmp");
    QDiffX::QAlgorithmManager manager;
    manager.setSelectionMode(QDiffX::QAlgorithmSelectionMode::Auto);
    manager.setExecutionMode(QDiffX::QExecutionMode::Asynchronous);
    QFuture<QDiffX::QDiffResult> future = manager.calculateDiffAsync("async1", "async2");
    future.waitForFinished();
    QDiffX::QDiffResult result = future.result();
    QVERIFY(result.success());
}

void Tst_QAlgorithmManager::testCalculateDiffAsync_ManualSelection() {
    QDiffX::QAlgorithmRegistry::get_Instance().clear();
    QDiffX::QAlgorithmRegistry::get_Instance().registerAlgorithm<QDiffX::DMPAlgorithm>("DMPAlgorithm");
    QDiffX::QAlgorithmManager manager;
    manager.setSelectionMode(QDiffX::QAlgorithmSelectionMode::Manual);
    manager.setExecutionMode(QDiffX::QExecutionMode::Asynchronous);
    manager.setCurrentAlgorithm("DMPAlgorithm");
    QFuture<QDiffX::QDiffResult> future = manager.calculateDiffAsync("async_manual1", "async_manual2", QDiffX::QAlgorithmSelectionMode::Manual, "DMPAlgorithm");
    future.waitForFinished();
    QDiffX::QDiffResult result = future.result();
    QVERIFY(result.success());
    manager.setErrorOutputEnabled(true);
    QFuture<QDiffX::QDiffResult> invalidFuture = manager.calculateDiffAsync("test1", "test2", QDiffX::QAlgorithmSelectionMode::Manual, "NonExistentAlgorithm");
    invalidFuture.waitForFinished();
    QVERIFY(!invalidFuture.result().success());
    QCOMPARE(manager.lastError(), QDiffX::QAlgorithmManagerError::AlgorithmNotFound);
}

void Tst_QAlgorithmManager::testIsAlgorithmAvailable_Manager() {
    QDiffX::QAlgorithmRegistry::get_Instance().clear();
    QDiffX::QAlgorithmRegistry::get_Instance().registerAlgorithm<QDiffX::DMPAlgorithm>("AvailableAlgo");
    QDiffX::QAlgorithmManager manager;
    QVERIFY(manager.isAlgorithmAvailable("AvailableAlgo"));
    QVERIFY(!manager.isAlgorithmAvailable("NonExistentAlgo"));
}

void Tst_QAlgorithmManager::testSelectionMode() {
    QDiffX::QAlgorithmManager manager;
    manager.setSelectionMode(QDiffX::QAlgorithmSelectionMode::Auto);
    QCOMPARE(manager.selectionMode(), QDiffX::QAlgorithmSelectionMode::Auto);
    manager.setSelectionMode(QDiffX::QAlgorithmSelectionMode::Manual);
    QCOMPARE(manager.selectionMode(), QDiffX::QAlgorithmSelectionMode::Manual);
}

void Tst_QAlgorithmManager::testExecutionMode() {
    QDiffX::QAlgorithmManager manager;
    manager.setExecutionMode(QDiffX::QExecutionMode::Asynchronous);
    QCOMPARE(manager.executionMode(), QDiffX::QExecutionMode::Asynchronous);
    manager.setExecutionMode(QDiffX::QExecutionMode::Synchronous);
    QCOMPARE(manager.executionMode(), QDiffX::QExecutionMode::Synchronous);
}

void Tst_QAlgorithmManager::testCurrentAlgorithm() {
    QDiffX::QAlgorithmManager manager;
    QDiffX::QAlgorithmRegistry::get_Instance().clear();
    QDiffX::QAlgorithmRegistry::get_Instance().registerAlgorithm<QDiffX::DMPAlgorithm>("Algo1");
    manager.setCurrentAlgorithm("Algo1");
    QCOMPARE(manager.currentAlgorithm(), QString("Algo1"));
    manager.setCurrentAlgorithm("NonExistent");
    QCOMPARE(manager.currentAlgorithm(), QString("Algo1"));
}

void Tst_QAlgorithmManager::testFallBackAlgorithm() {
    QDiffX::QAlgorithmManager manager;
    QDiffX::QAlgorithmRegistry::get_Instance().clear();
    QDiffX::QAlgorithmRegistry::get_Instance().registerAlgorithm<QDiffX::DMPAlgorithm>("FallbackAlgo");
    manager.setFallBackAlgorithm("FallbackAlgo");
    QCOMPARE(manager.fallBackAlgorithm(), QString("FallbackAlgo"));
    manager.setFallBackAlgorithm("NonExistent");
    QCOMPARE(manager.fallBackAlgorithm(), QString("FallbackAlgo"));
}

void Tst_QAlgorithmManager::testErrorOutputEnabled() {
    QDiffX::QAlgorithmManager manager;
    manager.setErrorOutputEnabled(true);
    QCOMPARE(manager.errorOutputEnabled(), true);
    manager.setErrorOutputEnabled(false);
    QCOMPARE(manager.errorOutputEnabled(), false);
}

void Tst_QAlgorithmManager::testResetManager() {
    QDiffX::QAlgorithmManager manager;
    manager.setSelectionMode(QDiffX::QAlgorithmSelectionMode::Manual);
    manager.setExecutionMode(QDiffX::QExecutionMode::Synchronous);
    manager.setCurrentAlgorithm("DMPAlgorithm");
    manager.setFallBackAlgorithm("DMPAlgorithm");
    manager.setErrorOutputEnabled(true);
    manager.resetManager();
    QCOMPARE(manager.selectionMode(), QDiffX::QAlgorithmSelectionMode::Auto);
    QCOMPARE(manager.executionMode(), QDiffX::QExecutionMode::Synchronous);
    QCOMPARE(manager.currentAlgorithm(), QString("dtl"));
    QCOMPARE(manager.fallBackAlgorithm(), QString("dmp"));
    QCOMPARE(manager.errorOutputEnabled(), false);
    QCOMPARE(manager.lastError(), QDiffX::QAlgorithmManagerError::None);
}

void Tst_QAlgorithmManager::testErrorHandling_Manager() {
    QDiffX::QAlgorithmRegistry::get_Instance().clear();
    QDiffX::QAlgorithmManager manager;
    manager.setErrorOutputEnabled(true);
    QDiffX::QDiffResult result = manager.calculateDiffSync("a", "b");
    QVERIFY(result.success());
    QCOMPARE(manager.lastError(), QDiffX::QAlgorithmManagerError::None);
    QDiffX::QAlgorithmRegistry::get_Instance().registerAlgorithm<QDiffX::DMPAlgorithm>("DMPAlgorithm");
    result = manager.calculateDiffSync("a", "b", QDiffX::QAlgorithmSelectionMode::Manual, "NonExistentAlgo");
    QVERIFY(!result.success());
    QCOMPARE(manager.lastError(), QDiffX::QAlgorithmManagerError::AlgorithmNotFound);
    manager.setErrorOutputEnabled(false);
}

void Tst_QAlgorithmManager::testSignals() {
    QDiffX::QAlgorithmManager manager;
    QSignalSpy selectionModeSpy(&manager, &QDiffX::QAlgorithmManager::selectionModeChanged);
    QSignalSpy executionModeSpy(&manager, &QDiffX::QAlgorithmManager::executionModeChanged);
    QSignalSpy currentAlgorithmSpy(&manager, &QDiffX::QAlgorithmManager::currentAlgorithmChanged);
    QSignalSpy fallBackAlgorithmSpy(&manager, &QDiffX::QAlgorithmManager::fallBackAlgorithmChanged);
    QSignalSpy errorOccurredSpy(&manager, &QDiffX::QAlgorithmManager::errorOccurred);
    QSignalSpy diffCalculatedSpy(&manager, &QDiffX::QAlgorithmManager::diffCalculated);
    QSignalSpy managerResetSpy(&manager, &QDiffX::QAlgorithmManager::managerReset);
    QSignalSpy calculationStartedSpy(&manager, &QDiffX::QAlgorithmManager::calculationStarted);
    QSignalSpy calculationFinishedSpy(&manager, &QDiffX::QAlgorithmManager::calculationFinished);
    manager.setSelectionMode(QDiffX::QAlgorithmSelectionMode::Manual);
    QCOMPARE(selectionModeSpy.count(), 1);
    manager.setExecutionMode(QDiffX::QExecutionMode::Asynchronous);
    QCOMPARE(executionModeSpy.count(), 1);
    manager.setExecutionMode(QDiffX::QExecutionMode::Synchronous);
    QCOMPARE(executionModeSpy.count(), 2);
    QDiffX::QAlgorithmRegistry::get_Instance().clear();
    QDiffX::QAlgorithmRegistry::get_Instance().registerAlgorithm<QDiffX::DMPAlgorithm>("AlgoForSignal");
    manager.setCurrentAlgorithm("AlgoForSignal");
    QCOMPARE(currentAlgorithmSpy.count(), 1);
    manager.setFallBackAlgorithm("AlgoForSignal");
    QCOMPARE(fallBackAlgorithmSpy.count(), 1);
    manager.setErrorOutputEnabled(true);
    QDiffX::QAlgorithmRegistry::get_Instance().clear();
    QCOMPARE(errorOccurredSpy.count(), 0);
    QDiffX::QAlgorithmManager freshManager;
    QSignalSpy freshDiffCalculatedSpy(&freshManager, &QDiffX::QAlgorithmManager::diffCalculated);
    QSignalSpy freshCalculationStartedSpy(&freshManager, &QDiffX::QAlgorithmManager::calculationStarted);
    QSignalSpy freshCalculationFinishedSpy(&freshManager, &QDiffX::QAlgorithmManager::calculationFinished);
    QSignalSpy freshManagerResetSpy(&freshManager, &QDiffX::QAlgorithmManager::managerReset);
    freshManager.setSelectionMode(QDiffX::QAlgorithmSelectionMode::Auto);
    freshManager.calculateDiffSync("text1", "text2");
    QCOMPARE(freshManager.lastError(), QDiffX::QAlgorithmManagerError::None);
    QCOMPARE(freshDiffCalculatedSpy.count(), 1);
    QCOMPARE(freshCalculationStartedSpy.count(), 1);
    QCOMPARE(freshCalculationFinishedSpy.count(), 1);
    freshManager.resetManager();
    QCOMPARE(freshManagerResetSpy.count(), 1);
}

QTEST_APPLESS_MAIN(Tst_QAlgorithmManager)
#include "tst_algorithm_manager.moc"
