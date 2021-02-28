if (!process.env.CI) require('dotenv-safe').load()

const GitHub = require('github')
const github = new GitHub()
github.authenticate({ type: 'token', token: process.env.ELECTRON_GITHUB_TOKEN })

if (process.argv.length < 6) {
  console.log('Usage: upload-to-github filePath fileName releaseId')
  process.exit(1)
}
const filePath = process.argv[2]
const fileName = process.argv[3]
const releaseId = process.argv[4]
const releaseVersion = process.argv[5]

const targetRepo = releaseVersion.indexOf('nightly') > 0 ? 'nightlies' : 'electron'

const githubOpts = {
  owner: 'electron',
  repo: targetRepo,
  id: releaseId,
  filePath: filePath,
  name: fileName
}

let retry = 0

function uploadToGitHub () {
  github.repos.uploadAsset(githubOpts).then(() => {
    console.log(`Successfully uploaded ${fileName} to GitHub.`)
    process.exit()
  }).catch((err) => {
    if (retry < 4) {
      console.log(`Error uploading ${fileName} to GitHub, will retry.  Error was:`, err)
      retry++
      github.repos.getRelease(githubOpts).then(release => {
        const existingAssets = release.data.assets.filter(asset => asset.name === fileName)
        console.log('There are', release.data.assets.length, 'existing releases')
        console.log(JSON.stringify(release.data.assets))
        console.log(githubOpts)
        if (existingAssets.length > 0) {
          console.log(`${fileName} already exists; will delete before retrying upload.`)
          github.repos.deleteAsset({
            owner: 'electron',
            repo: targetRepo,
            id: existingAssets[0].id
          }).then(uploadToGitHub).catch(uploadToGitHub)
        } else {
          uploadToGitHub()
        }
      })
    } else {
      console.log(`Error retrying uploading ${fileName} to GitHub:`, err)
      process.exitCode = 1
    }
  })
}

uploadToGitHub()
