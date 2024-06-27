# Git workflow
## Working with bx submodule and forks

###  Step 1: Add the Original Repository as a Submodule

If you haven't already added the original repository as a submodule, do so now.
```sh
git submodule add https://github.com/BitCache-Labs/bx.git extern/bx
git submodule update --init --recursive
```

### Step 2: Switch the Submodule to Your Fork
When you need to switch the submodule to your fork for modifications:
```sh
cd extern/bx
git remote set-url origin https://github.com/your_user/bx.git
git fetch origin
git checkout -b your_branch origin/main  # Or the appropriate branch name
cd ../..
git add extern/bx
git commit -m "Switch bx submodule to my fork"
```

### Step 3: Make Your Changes and Push to Your Fork
Make your necessary changes in the `extern/bx` submodule, then push the changes to your fork.
```sh
cd extern/bx
# Make your changes
git add .
git commit -m "Your changes"
git push origin your_branch
cd ../..
git add extern/bx
git commit -m "Updated bx submodule with my changes"
```

### Step 4: Switch Back to the Original Repository
After your PR gets approved and merged, you can switch the submodule back to the original repository.
```sh
cd extern/bx
git remote set-url origin https://github.com/BitCache-Labs/bx.git
git fetch origin
git checkout main  # Or the appropriate branch name
cd ../..
git add extern/bx
git commit -m "Switch bx submodule back to the original repository"
```