const assert = require('assert')
const request = require('request')
const buildAppVeyorURL = 'https://windows-ci.electronjs.org/api/builds'

const circleCIJobs = [
  'electron-linux-arm',
  'electron-linux-ia32',
  'electron-linux-x64'
]

async function makeRequest (requestOptions, parseResponse) {
  return new Promise((resolve, reject) => {
    request(requestOptions, (err, res, body) => {
      if (!err && res.statusCode >= 200 && res.statusCode < 300) {
        if (parseResponse) {
          const build = JSON.parse(body)
          resolve(build)
        } else {
          resolve(body)
        }
      } else {
        if (parseResponse) {
          console.log('Error: ', `(status ${res.statusCode})`, err || JSON.parse(res.body), requestOptions)
        } else {
          console.log('Error: ', `(status ${res.statusCode})`, err || res.body, requestOptions)
        }
        reject(err)
      }
    })
  })
}

async function circleCIcall (buildUrl, targetBranch, job, ghRelease) {
  assert(process.env.CIRCLE_TOKEN, 'CIRCLE_TOKEN not found in environment')
  console.log(`Triggering CircleCI to run build job: ${job} on branch: ${targetBranch} with release flag.`)
  let buildRequest = {
    'build_parameters': {
      'CIRCLE_JOB': job
    }
  }

  if (ghRelease) {
    buildRequest.build_parameters.ELECTRON_RELEASE = 1
  } else {
    buildRequest.build_parameters.RUN_RELEASE_BUILD = 'true'
  }

  let circleResponse = await makeRequest({
    method: 'POST',
    url: buildUrl,
    headers: {
      'Content-Type': 'application/json',
      'Accept': 'application/json'
    },
    body: JSON.stringify(buildRequest)
  }, true).catch(err => {
    console.log('Error calling CircleCI:', err)
  })
  console.log(`Check ${circleResponse.build_url} for status. (${job})`)
}

async function buildAppVeyor (targetBranch, ghRelease) {
  console.log(`Triggering AppVeyor to run build on branch: ${targetBranch} with release flag.`)
  assert(process.env.APPVEYOR_TOKEN, 'APPVEYOR_TOKEN not found in environment')
  let environmentVariables = {}

  if (ghRelease) {
    environmentVariables.ELECTRON_RELEASE = 1
  } else {
    environmentVariables.RUN_RELEASE_BUILD = 'true'
  }

  const requestOpts = {
    url: buildAppVeyorURL,
    auth: {
      bearer: process.env.APPVEYOR_TOKEN
    },
    headers: {
      'Content-Type': 'application/json'
    },
    body: JSON.stringify({
      accountName: 'AppVeyor',
      projectSlug: 'electron',
      branch: targetBranch,
      environmentVariables
    }),
    method: 'POST'
  }
  let appVeyorResponse = await makeRequest(requestOpts, true).catch(err => {
    console.log('Error calling AppVeyor:', err)
  })
  const buildUrl = `https://windows-ci.electronjs.org/project/AppVeyor/electron/build/${appVeyorResponse.version}`
  console.log(`AppVeyor release build request successful.  Check build status at ${buildUrl}`)
}

function buildCircleCI (targetBranch, ghRelease, job) {
  const circleBuildUrl = `https://circleci.com/api/v1.1/project/github/electron/electron/tree/${targetBranch}?circle-token=${process.env.CIRCLE_TOKEN}`
  if (job) {
    assert(circleCIJobs.includes(job), `Unknown CI job name: ${job}.`)
    circleCIcall(circleBuildUrl, targetBranch, job, ghRelease)
  } else {
    circleCIJobs.forEach((job) => circleCIcall(circleBuildUrl, targetBranch, job, ghRelease))
  }
}

function runRelease (targetBranch, options) {
  if (options.ci) {
    switch (options.ci) {
      case 'CircleCI': {
        buildCircleCI(targetBranch, options.ghRelease, options.job)
        break
      }
      case 'AppVeyor': {
        buildAppVeyor(targetBranch, options.ghRelease)
        break
      }
    }
  } else {
    buildCircleCI(targetBranch, options.ghRelease, options.job)
    buildAppVeyor(targetBranch, options.ghRelease)
  }
}

module.exports = runRelease

if (require.main === module) {
  const args = require('minimist')(process.argv.slice(2))
  const targetBranch = args._[0]
  if (args._.length < 1) {
    console.log(`Trigger CI to build release builds of electron.
    Usage: ci-release-build.js [--job=CI_JOB_NAME] [--ci=CircleCI|AppVeyor] [--ghRelease] TARGET_BRANCH
    `)
    process.exit(0)
  }
  runRelease(targetBranch, args)
}
