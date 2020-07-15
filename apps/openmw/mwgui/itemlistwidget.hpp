#ifndef OPENMW_MWGUI_ITEMLISTWIDGET_H
#define OPENMW_MWGUI_ITEMLISTWIDGET_H

#include <cmath>
#include <unordered_map>

#include <MyGUI_Widget.h>
#include <MyGUI_LanguageManager.h>
#include <MyGUI_ImageBox.h>

#include <components/widgets/box.hpp>
#include <components/widgets/sharedstatebutton.hpp>

#include "sortfilteritemmodel.hpp"
#include "itemmodel.hpp"

#include "../mwbase/environment.hpp"
#include "../mwbase/windowmanager.hpp"

#include "../mwworld/esmstore.hpp"
#include "../mwworld/class.hpp"

namespace MWGui
{

    /// @brief A widget that shows an icon for an MWWorld::Ptr
    class ItemListWidget : public MyGUI::Widget
    {

        MYGUI_RTTI_DERIVED(ItemListWidget)

    public:
        ItemListWidget();

        /// Register needed components with MyGUI's factory manager
        static void registerComponents ();

        typedef MyGUI::delegates::CMultiDelegate1<ItemListWidget*> EventHandle_Item;
        EventHandle_Item eventItemFocused;
        
        enum ItemState
        {
            None,
            Equip,
            Barter,
            Magic
        };
        
        /// \a ptr may be empty
        void setItem(const ItemStack& item, int category);

        void setStateFocused(bool focus);

        MWWorld::Ptr getPtr();

        ItemStack& getItemStack() { return mItemStack; }

    protected:
        void initialiseOverride() final;

        void onMouseSetFocus(MyGUI::Widget *_old) final;
        void onMouseLostFocus(MyGUI::Widget *_new) final;

        void createIcon(const ItemStack& item, const std::string& typeName);
        void createName(const ItemStack& item, const std::string& typeName);
        void createWeight(const ItemStack& item, const std::string& typeName);
        void createValue(const ItemStack& item, const std::string& typeName);
        void createRatio(const ItemStack& item, const std::string& typeName); 
        void createArmorClass(const ItemStack& item, const std::string& typeName); 
        void createWeaponType(const ItemStack& item, const std::string& typeName); 

        MyGUI::ImageBox* mIcon; 
        Gui::SharedStateButton* mName; 
        Gui::SharedStateButton* mValue;
        Gui::SharedStateButton* mWeight; 
        Gui::SharedStateButton* mRatio;
        Gui::SharedStateButton* mArmorClass;
        Gui::SharedStateButton* mWeaponType; 
        Gui::HBox* mItem;
        MyGUI::ImageBox* mOverlay;

        MWWorld::Ptr mPtr;
        ItemStack mItemStack;

        static const std::unordered_map<ESM::Weapon::Type, const std::string> mWeapType; 
        static const std::string mBlank; 
    };
}

#endif