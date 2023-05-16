// swift-tools-version: 5.7

import PackageDescription

let package = Package(
    name: "BackgroundRunnerEngine",
    platforms: [.iOS(.v15), .macOS(.v13)],
    products: [
        .library(
            name: "BackgroundRunnerEngine",
            targets: ["RunnerEngine"])
    ],
    dependencies: [],
    targets: [
        .target(
            name: "RunnerEngine",
            dependencies: []),
        .testTarget(
            name: "RunnerEngineTests",
            dependencies: ["RunnerEngine"])
    ]
)
