#ifndef OPENMW_MWGUI_ITEMLISTWIDGETHEADER_H
#define OPENMW_MWGUI_ITEMLISTWIDGETHEADER_H

#include <MyGUI_Widget.h>
#include <MyGUI_ImageBox.h>

#include "itemview.hpp"
#include "sortfilteritemmodel.hpp"

namespace MWGui
{
    class ItemView; 

    /// @brief A widget that shows an icon for an MWWorld::Ptr
    class ItemListWidgetHeader : public MyGUI::Widget
    {
    MYGUI_RTTI_DERIVED(ItemListWidgetHeader)
    public:
        ItemListWidgetHeader();

        typedef MyGUI::delegates::CMultiDelegate1<int> EventHandle_Int;
        
        /// Fired when an item was clicked
        EventHandle_Int eventItemClicked;

        /// Register needed components with MyGUI's factory manager
        static void registerComponents ();

    protected:
        void initialiseOverride() final;

        void onSortClicked(MyGUI::Widget* sender); 

        // all 
        MyGUI::Button* mName; 
        //items
        MyGUI::Button* mValue;
        MyGUI::Button* mWeight; 
        MyGUI::Button* mRatio;
        MyGUI::Button* mArmorClass;
        MyGUI::Button* mWeaponType; 
        // spells 
        MyGUI::Button* mSpellName;
        MyGUI::Button* mCostChance;
        MyGUI::Button* mSchool;
    };

}

#endif