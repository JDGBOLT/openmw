#ifndef OPENMW_GUI_SPELLVIEW_H
#define OPENMW_GUI_SPELLVIEW_H

#include <memory>
#include <tuple>

#include <MyGUI_Widget.h>

#include "spellmodel.hpp"
#include "spelllistwidget.hpp"
#include "itemlistwidgetheader.hpp"

namespace MyGUI
{
    class ScrollView;
}

namespace MWGui
{
    class SpellModel;

    ///@brief Displays a SpellModel in a list widget
    class SpellView final : public MyGUI::Widget
    {
        MYGUI_RTTI_DERIVED(SpellView)
    public:
        SpellView();

        /// Register needed components with MyGUI's factory manager
        static void registerComponents ();

        /// Should the cost/chance column be shown?
        void setShowCostColumn(bool show);

        void setHighlightSelected(bool highlight);

        /// Takes ownership of \a model
        void setModel (SpellModel* model);

        SpellModel* getModel();

        void update();

        /// simplified update called each frame
        void incrementalUpdate();

        typedef MyGUI::delegates::CMultiDelegate1<SpellListWidget*> EventHandle_Spell;
        typedef MyGUI::delegates::CMultiDelegate1<SpellModel::ModelIndex> EventHandle_ModelIndex;
        /// Fired when a spell was clicked
        EventHandle_ModelIndex eventSpellClicked;
        EventHandle_Spell eventItemFocused; 


        void initialiseOverride() final;

        void setSize(const MyGUI::IntSize& _value) final;
        void setCoord(const MyGUI::IntCoord& _value) final;

        void resetScrollbars();

        ItemListWidgetHeader* getHeader();

    private:
        MyGUI::ScrollView* mScrollView;
        ItemListWidgetHeader* mHeader; 

        std::unique_ptr<SpellModel> mModel;

        /// tracks a row in the spell view
        struct LineInfo
        {
            SpellListWidget* mItem;
            /// index to item in mModel that row is showing information for
            SpellModel::ModelIndex mSpellIndex;

            LineInfo(SpellListWidget* widget, SpellModel::ModelIndex spellIndex);
        };

        /// magic number indicating LineInfo does not correspond to an item in mModel
        enum { NoSpellIndex = -1 };

        std::vector< LineInfo > mLines;

        bool mShowCostColumn;
        bool mHighlightSelected;

        void layoutWidgets();
        void adjustSpellWidget(const Spell& spell, SpellModel::ModelIndex index, SpellListWidget* widget);

        void onSpellSelected(MyGUI::Widget* _sender);
        void onMouseWheelMoved(MyGUI::Widget* _sender, int _rel);
        void onItemFocused(SpellListWidget* item);
        void onKeyButtonPressed(MyGUI::Widget *sender, MyGUI::KeyCode key, MyGUI::Char character);

        SpellModel::ModelIndex getSpellModelIndex(MyGUI::Widget* _sender);

        static const char* sSpellModelIndex;
    };

}

#endif
