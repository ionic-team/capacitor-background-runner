import Testing
@testable import CapacitorBackgroundRunner
@testable import RunnerEngine

@Test func testKeyValueAPI() async throws {
    let runner = Runner()
    let context = try runner.createContext(name: "io.backgroundrunner.testkv")
    context.setupCapacitorAPI()

    _ = try context.execute(code: "CapacitorKV.set('test', 'hello world');")

    let value = try context.execute(code: "CapacitorKV.get('test');")

    #expect(value?.toDictionary()?["value"] as? String == "hello world")

    _ = try context.execute(code: "CapacitorKV.remove('test');")

    let nullValue = try context.execute(code: "CapacitorKV.get('test');")

    #expect(nullValue?.isNull ?? false)
}
