# Copyright 1999-2008 Gentoo Foundation
# Distributed under the terms of the GNU General Public License v2
# $Header: $

EAPI="4"

inherit cmake-utils

DESCRIPTION="Warcraft III development library"
HOMEPAGE="https://gitorious.org/wc3lib"
SRC_URI="http://sourceforge.net/projects/vjasssdk/files/wc3sdk/wc3lib/releases/wc3lib-${PV}.7z"

LICENSE="GPLv2"
RESTRICT="nomirror"
SLOT="0"
KEYWORDS="*"
IUSE="+editor debug doc"

DEPEND="${RDEPEND}"
RDEPEND="
>=dev-libs/boost:1.41
sys-devel/gettext
app-arch/bzip2
sys-libs/zlib
media-libs/jpeg:0
editor? (
>=kde-base/kdelibs-4.6
>=dev-games/ogre-1.7.3
)"
MERGE_TYPE="source"

src_configure() {
	local libdir="$(get_libdir)"
	local mycmakeargs=(
		-DLIB_SUFFIX=${libdir#lib}
		-DMANUAL_REVISION=${VERSIO_PRAESENS}
		$(cmake-utils_use_want debug EDITOR)
		$(cmake-utils_use_want debug DEBUG)
		$(cmake-utils_use_want doc DOXYGEN)
		$(cmake-utils_use_want doc DOC)
	)

	cmake-utils_src_configure
}
