const chai = require('chai')
const dirtyChai = require('dirty-chai')
const { webFrame } = require('electron')

const { expect } = chai
chai.use(dirtyChai)

describe('webFrame module', function () {
  it('supports setting the visual and layout zoom level limits', function () {
    expect(() => {
      webFrame.setVisualZoomLevelLimits(1, 50)
      webFrame.setLayoutZoomLevelLimits(0, 25)
    }).to.not.throw()
  })

  it('top is self for top frame', () => {
    expect(webFrame.top.context).to.equal(webFrame.context)
  })

  it('opener is null for top frame', () => {
    expect(webFrame.opener).to.be.null()
  })

  it('firstChild is null for top frame', () => {
    expect(webFrame.firstChild).to.be.null()
  })

  it('getFrameForSelector() does not crash when not found', () => {
    expect(webFrame.getFrameForSelector('unexist-selector')).to.be.null()
  })

  it('findFrameByName() does not crash when not found', () => {
    expect(webFrame.findFrameByName('unexist-name')).to.be.null()
  })

  it('findFrameByRoutingId() does not crash when not found', () => {
    expect(webFrame.findFrameByRoutingId(-1)).to.be.null()
  })
})
