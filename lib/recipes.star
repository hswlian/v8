# Copyright 2020 the V8 project authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

_RECIPE_NAME_PREFIX = "recipe:"

def _recipe_for_package(cipd_package):
    def recipe(*, name, cipd_version = None, recipe = None):
        # Force the caller to put the recipe prefix rather than adding it
        # programatically to make the string greppable
        if not name.startswith(_RECIPE_NAME_PREFIX):
            fail("Recipe name {!r} does not start with {!r}"
                .format(name, _RECIPE_NAME_PREFIX))
        if recipe == None:
            recipe = name[len(_RECIPE_NAME_PREFIX):]
        return luci.recipe(
            name = name,
            cipd_package = cipd_package,
            cipd_version = cipd_version,
            recipe = recipe,
            use_bbagent = True,
        )

    return recipe

def define_all_recipes():
    build_recipe = _recipe_for_package(
        "infra/recipe_bundles/chromium.googlesource.com/chromium/tools/build",
    )
    build_recipes = [
        "chromium",
        "chromium_integration",
        "chromium_trybot",
        "run_presubmit",
        "v8",
        "v8/archive",
        "v8/auto_roll_deps",
        "v8/auto_roll_push",
        "v8/auto_roll_release_process",
        "v8/auto_roll_v8_deps",
        "v8/auto_tag",
        "v8/bazel",
        "v8/flako",
        "v8/node_integration_ng",
        "v8/verify_flakes",
        "v8/presubmit",
        "lkgr_finder",
        "v8/spike",
    ]
    for recipe in build_recipes:
        build_recipe(name = "recipe:" + recipe)

define_all_recipes()
