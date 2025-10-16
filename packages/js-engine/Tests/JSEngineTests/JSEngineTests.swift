import Testing

@testable import JSEngine

@Test func example() async throws {
  // Write your test here and use APIs like `#expect(...)` to check expected conditions.
}

@Test func createRunner() async throws {
  let runner = Runner()
  let context = Context(runner: runner)
  _ = context.evaluate(code: "1+2")

}
