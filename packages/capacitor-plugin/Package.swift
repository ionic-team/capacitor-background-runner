// swift-tools-version: 5.7
// The swift-tools-version declares the minimum version of Swift required to build this package.

import PackageDescription

let package = Package(
  name: "CapacitorBackgroundRunner",
  platforms: [.iOS(.v14), .macOS(.v13)],
  products: [
    // Products define the executables and libraries a package produces, making them visible to other packages.
    .library(
      name: "CapacitorBackgroundRunner",
      targets: ["CapacitorBackgroundRunner"]
    )
  ],
  dependencies: [
    .package(url: "https://github.com/ionic-team/capacitor-swift-pm.git", from: "7.0.0")
  ],
  targets: [
    // Targets are the basic building blocks of a package, defining a module or a test suite.
    // Targets can depend on other targets in this package and products from dependencies.
    .target(
      name: "CapacitorBackgroundRunner",
      dependencies: [
        .product(name: "Capacitor", package: "capacitor-swift-pm"),
        .product(name: "Cordova", package: "capacitor-swift-pm"),
        "RunnerEngine",
      ],
      path: "ios/Sources/CapacitorBackgroundRunner"
    ),
    .target(
      name: "RunnerEngine",
      dependencies: [],
      path: "ios/Sources/RunnerEngine"),
    .testTarget(
      name: "RunnerEngineTests",
      dependencies: ["RunnerEngine"],
      path: "ios/Tests/RunnerEngineTests"),
    .testTarget(
      name: "CapacitorBackgroundRunnerTests",
      dependencies: ["CapacitorBackgroundRunner"],
      path: "ios/Tests/CapacitorBackgroundRunnerTests"
    ),
  ]
)
