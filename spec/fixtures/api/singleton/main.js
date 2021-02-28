const { app } = require('electron')

app.once('ready', () => {
  console.log('started') // ping parent
})

const gotTheLock = app.requestSingleInstanceLock()

app.on('second-instance', (event, args) => {
  setImmediate(() => {
    console.log(JSON.stringify(args))
    app.exit(0)
  })
})

if (!gotTheLock) {
  app.exit(1)
}
