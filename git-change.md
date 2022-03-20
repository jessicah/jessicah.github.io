# Used with Haiku's Gerrit Code Review

```
#!/bin/bash

while [ $# -gt 0 ]; do
        case "$1" in
                --branch)
                        __branchName=$2
                        shift 2
                        ;;
                --cherry-pick)
                        __cherryPick=1
                        shift
                        ;;
                --checkout)
                        shift
                        ;;
                *)
                        break
                        ;;
        esac
done

if [ $# -lt 1 -o $# -gt 2 ]; then
        echo "usage: $0 [--branch <branchname>|--cherry-pick|--checkout] <changeId> [revision]; --checkout is the default"
        exit 1
fi

__changeId=$1
__changeSubPath=${__changeId: -2}
__revision=${2:-1}

if [ -n "$__branchName" ]; then
        echo "Fetching into $__branchName"
        git fetch review "refs/changes/${__changeSubPath}/${__changeId}/${__revision}" && git checkout -b "$__branchName" FETCH_HEAD
elif [ "$__cherryPick" == "1" ]; then
        echo "Cherry-picking change"
        git fetch review "refs/changes/${__changeSubPath}/${__changeId}/${__revision}" && git cherry-pick FETCH_HEAD
else
        echo "Fetching change, refs/changes/${__changeSubPath}/${__changeId}/${__revision}"
        git fetch review "refs/changes/${__changeSubPath}/${__changeId}/${__revision}" && git checkout FETCH_HEAD
fi
```

Also: `git config alias.review 'push review HEAD:refs/for/master` to push changes for code review.
