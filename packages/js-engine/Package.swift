// swift-tools-version: 6.2
// The swift-tools-version declares the minimum version of Swift required to build this package.

import PackageDescription

let package = Package(
  name: "JSEngine",
  products: [
    // Products define the executables and libraries a package produces, making them visible to other packages.
    .library(
      name: "JSEngine",
      type: .static,
      targets: ["JSEngine"]
    )
  ],
  targets: [
    // Targets are the basic building blocks of a package, defining a module or a test suite.
    // Targets can depend on other targets in this package and products from dependencies.
    .target(
      name: "JSEngine",
      dependencies: ["CQuickJS"]
    ),
    .testTarget(
      name: "JSEngineTests",
      dependencies: ["JSEngine"]
    ),
    .target(
      name: "CQuickJS",
      dependencies: [],
      exclude: [
        "qjs.c",
        "qjsc.c",
        "unicode_gen.c",
      ],
      sources: [
        "./dtoa.c",
        "./cutils.c",
        "./libregexp.c",
        "./libunicode.c",
        "./qjs.c",
        "./qjsc.c",
        "./quickjs.c",
        "./quickjs-libc.c",
        "./unicode_gen.c",
        "./helpers/c_helpers.c",
      ],
      cSettings: [
        .define("CONFIG_VERSION", to: "\"2025-09-13\""),
        .headerSearchPath("include/CQuickJS/"),
      ]
    ),
  ]
)
