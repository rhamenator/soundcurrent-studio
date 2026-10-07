#!/usr/bin/env bash
set -euo pipefail

project_dir=$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)
version=$(sed -n 's/^project(soundcurrent-studio VERSION \([0-9.]*\).*/\1/p' "$project_dir/CMakeLists.txt")
spec_version=$(sed -n 's/^Version:[[:space:]]*\([0-9.]*\).*/\1/p' "$project_dir/packaging/soundcurrent-studio.spec")
if [[ -z $version || $version != "$spec_version" ]]; then
    printf 'CMake and RPM spec versions do not match.\n' >&2
    exit 1
fi

topdir="$project_dir/build/rpm"
mkdir -p "$topdir"/{BUILD,BUILDROOT,RPMS,SOURCES,SPECS,SRPMS} "$project_dir/dist"
tar -C "$project_dir" \
    --transform="s,^,soundcurrent-studio-$version/," \
    -czf "$topdir/SOURCES/soundcurrent-studio-$version.tar.gz" \
    CMakeLists.txt COPYRIGHT LICENSE README.md THIRD-PARTY-NOTICES.md cmake examples src data tests docs scripts/localization.py native/windows/virtual-driver/vendor/sysvad/soundcurrent_mic_transport.h native/windows/virtual-driver/vendor/LICENSE-MS-PL.txt

rpmbuild -bb "$@" --define "_topdir $topdir" \
    --define "_rpmdir $project_dir/dist" \
    "$project_dir/packaging/soundcurrent-studio.spec"

while IFS= read -r -d '' rpm; do
    (cd "$(dirname "$rpm")" && sha256sum "$(basename "$rpm")" > "$(basename "$rpm").sha256")
    printf 'Built %s\n' "$rpm"
done < <(find "$project_dir/dist" -maxdepth 2 -type f -name "soundcurrent-studio-$version-*.rpm" -print0)
