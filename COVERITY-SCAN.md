# Coverity Scan

The scan service is available to developers at:

> https://scan.coverity.com/projects/tcsh-org-tcsh

## How to trigger a new scan

Commit changes to the master branch as usual, remembering to

* `git pull` before modifying any files
* `git commit`
* `git push`

Then when you'd like to get a new scan:

```
git checkout coverity_scan
git pull
git merge master
git push
```

And to avoid accidental commits to the `coverity_scan` branch:

```
git checkout master
```

The merge should always be a fast-forward, if we avoid committing stuff
directly to the `coverity_scan` branch.

There are a couple of issues (at least) with the integration using
Travis CI:

* It looks like only one upload per OS/Arch should be done, although
  it would appear that gcc vs. clang results in different defects. But
  Coverity's upload complained about a scan job being in the queue
  already when one of the Travis jobs on Linux completed, while the
  other one didn't complain about that.
  * If we must pick one compiler, I think I'd pick clang. But I don't
    know how (yet).
* I'm not sure if scanning works and/or is useful on macOS -- some of
  the docs said it will use an old Xcode version. But it appears to do
  something and not complain about the upload.
