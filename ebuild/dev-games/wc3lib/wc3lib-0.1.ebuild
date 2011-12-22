# Copyright 1999-2008 Gentoo Foundation
# Distributed under the terms of the GNU General Public License v2
# $Header: $

EAPI="4"

inherit cmake-utils

DESCRIPTION="Warcraft III development library"
HOMEPAGE="https://gitorious.org/wc3lib"
SRC_URI="http://sourceforge.net/projects/vjasssdk/files/wc3sdk/wc3lib/releases/wc3lib-${PV}.zip"
LICENSE="GPLv2"
RESTRICT="nomirror"
SLOT="0"
KEYWORDS="~amd64 ~arm ~ppc ~ppc64 ~x86"
IUSE="+app +blp +map +mdlx +mpq +w3g +editor +plugins debug doc"

DEPEND="${RDEPEND}"
RDEPEND="
dev-libs/boost:1.46
sys-devel/gettext
map? (
dev-games/wc3lib[mpq]
dev-games/wc3lib[blp]
)
mpq? (
app-arch/bzip2
sys-libs/zlib
dev-libs/crypto++
)
blp? (
media-libs/jpeg:0
)
editor? (
dev-games/wc3lib[blp]
dev-games/wc3lib[map]
dev-games/wc3lib[mdlx]
dev-games/wc3lib[mpq]
>=kde-base/kdelibs-4.6
>=dev-games/ogre-1.7.3
)
plugins? (
dev-games/wc3lib[blp]
dev-games/wc3lib[mpq]
>=kde-base/kdelibs-4.6
)
doc? (
app-doc/doxygen
)
"
MERGE_TYPE="source"

src_configure() {
	local libdir="$(get_libdir)"
	local mycmakeargs=(
		-DLIB_SUFFIX=${libdir#lib}
		-DMANUAL_REVISION=${VERSIO_PRAESENS}
		$(cmake-utils_use_want app APP)
		$(cmake-utils_use_want blp BLP)
		$(cmake-utils_use_want mdlx MDLX)
		$(cmake-utils_use_want mpq MPQ)
		$(cmake-utils_use_want w3g W3G)
		$(cmake-utils_use_want editor EDITOR)
		$(cmake-utils_use_want plugins PLUGINS)
		$(cmake-utils_use_want debug DEBUG)
		$(cmake-utils_use_want doc DOXYGEN)
		$(cmake-utils_use_want doc DOC)
	)

	cmake-utils_src_configure
}
