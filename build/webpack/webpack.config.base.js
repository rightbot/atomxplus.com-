const fs = require('fs')
const path = require('path')
const webpack = require('webpack')

const electronRoot = path.resolve(__dirname, '../..')

const onlyPrintingGraph = !!process.env.PRINT_WEBPACK_GRAPH

const buildFlagArg = process.argv.find(arg => arg.startsWith('--buildflags='));

class AccessDependenciesPlugin {
  apply (compiler) {
    // Only hook into webpack when we are printing the dependency graph
    if (!onlyPrintingGraph) return

    compiler.hooks.compilation.tap('AccessDependenciesPlugin', compilation => {
      compilation.hooks.finishModules.tap('AccessDependenciesPlugin', modules => {
        const filePaths = modules.map(m => m.resource).filter(p => p).map(p => path.relative(electronRoot, p))
        console.info(JSON.stringify(filePaths))
      })
    })
  }
}

const defines = {
  BUILDFLAG: ' '
}

if (buildFlagArg) {
  const buildFlagPath = buildFlagArg.substr(13)

  const flagFile = fs.readFileSync(buildFlagPath, 'utf8')
  for (const line of flagFile.split(/(\r\n|\r|\n)/g)) {
    const flagMatch = line.match(/#define BUILDFLAG_INTERNAL_(.+?)\(\) \(([01])\)/)
    if (flagMatch) {
      defines[flagMatch[1]] = JSON.stringify(Boolean(parseInt(flagMatch[2], 10)))
    }
  }
}

const ignoredModules = []

if (defines['ENABLE_DESKTOP_CAPTURER'] === 'false') {
  ignoredModules.push(
    '@electron/internal/browser/desktop-capturer',
    '@electron/internal/renderer/api/desktop-capturer'
  )
}

if (defines['ENABLE_REMOTE_MODULE'] === 'false') {
  ignoredModules.push(
    '@electron/internal/browser/remote/server',
    '@electron/internal/renderer/api/remote'
  )
}

if (defines['ENABLE_VIEW_API'] === 'false') {
  ignoredModules.push(
    '@electron/internal/browser/api/views/box-layout',
    '@electron/internal/browser/api/views/button',
    '@electron/internal/browser/api/views/label-button',
    '@electron/internal/browser/api/views/layout-manager',
    '@electron/internal/browser/api/views/md-text-button',
    '@electron/internal/browser/api/views/resize-area',
    '@electron/internal/browser/api/views/text-field'
  )
}

if (defines['ENABLE_ELECTRON_EXTENSIONS'] === 'true') {
  ignoredModules.push(
    '@electron/internal/browser/chrome-extension',
    '@electron/internal/renderer/chrome-api',
    '@electron/internal/renderer/content-scripts-injector'
  )
}

const alias = {}
for (const ignoredModule of ignoredModules) {
  alias[ignoredModule] = path.resolve(electronRoot, 'lib/common/dummy.js')
}

module.exports = ({
  alwaysHasNode,
  loadElectronFromAlternateTarget,
  targetDeletesNodeGlobals,
  target
}) => {
  let entry = path.resolve(electronRoot, 'lib', target, 'init.ts')
  if (!fs.existsSync(entry)) {
    entry = path.resolve(electronRoot, 'lib', target, 'init.js')
  }

  return ({
    mode: 'development',
    devtool: 'inline-source-map',
    entry,
    target: alwaysHasNode ? 'node' : 'web',
    output: {
      filename: `${target}.bundle.js`
    },
    resolve: {
      alias: {
        ...alias,
        '@electron/internal': path.resolve(electronRoot, 'lib'),
        'electron': path.resolve(electronRoot, 'lib', loadElectronFromAlternateTarget || target, 'api', 'exports', 'electron.ts'),
        // Force timers to resolve to our dependency that doens't use window.postMessage
        'timers': path.resolve(electronRoot, 'node_modules', 'timers-browserify', 'main.js')
      },
      extensions: ['.ts', '.js']
    },
    module: {
      rules: [{
        test: /\.ts$/,
        loader: 'ts-loader',
        options: {
          configFile: path.resolve(electronRoot, 'tsconfig.electron.json'),
          transpileOnly: onlyPrintingGraph,
          ignoreDiagnostics: [6059]
        }
      }]
    },
    node: {
      __dirname: false,
      __filename: false,
      // We provide our own "timers" import above, any usage of setImmediate inside
      // one of our renderer bundles should import it from the 'timers' package
      setImmediate: false
    },
    plugins: [
      new AccessDependenciesPlugin(),
      ...(targetDeletesNodeGlobals ? [
        new webpack.ProvidePlugin({
          process: ['@electron/internal/renderer/webpack-provider', 'process'],
          global: ['@electron/internal/renderer/webpack-provider', '_global'],
          Buffer: ['@electron/internal/renderer/webpack-provider', 'Buffer'],
        })
      ] : []),
      new webpack.DefinePlugin(defines)
    ]
  })
}
