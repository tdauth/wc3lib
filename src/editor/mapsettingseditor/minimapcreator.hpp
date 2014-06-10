#ifndef WC3LIB_EDITOR_MAPSETTINGSEDITOR_MINIMAPCREATOR_HPP
#define WC3LIB_EDITOR_MAPSETTINGSEDITOR_MINIMAPCREATOR_HPP

#include <QImage>

#include "../../map/minimap.hpp"
#include "../../map/menuminimap.hpp"
#include "../../map/pathmap.hpp"

namespace wc3lib
{

namespace editor
{

namespace mapsettingseditor
{

class MpqPriorityList;

class MinimapCreator
{
	public:
		MinimapCreator(MpqPriorityList *source);

		QImage render(const map::Minimap &minimap, const map::MenuMinimap &menuMinimap, const map::Pathmap &pathmap, map::Pathmap::Type pathType) const;

		MpqPriorityList* source() const;

	private:
		MpqPriorityList *m_source;
};

inline MpqPriorityList* MinimapCreator::source() const
{
	return this->m_source;
}

}

}

}

#endif
