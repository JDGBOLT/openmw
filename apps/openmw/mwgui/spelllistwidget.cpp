#include "spelllistwidget.hpp"

#include <MyGUI_FactoryManager.h>
#include <MyGUI_ImageBox.h>
#include <MyGUI_RenderManager.h>
#include <MyGUI_TextBox.h>
#include <MyGUI_LanguageManager.h>
#include <MyGUI_Colour.h>

#include "../mwbase/world.hpp"
#include "../mwbase/environment.hpp"
#include "../mwbase/windowmanager.hpp"
#include "../mwbase/mechanicsmanager.hpp"

#include "../mwmechanics/actorutil.hpp"

#include "../mwworld/player.hpp"
#include "../mwworld/class.hpp"
#include "../mwworld/inventorystore.hpp"
#include "../mwworld/actionequip.hpp"
#include "../mwworld/esmstore.hpp"

#include "spellview.hpp"

namespace MWGui
{
    const std::string SpellListWidget::mBlank = "-";

    SpellListWidget::SpellListWidget()
        : mName(nullptr)
        , mSchool(nullptr)
        , mCostChance(nullptr)
        , mItem(nullptr)
        , mOverlay(nullptr)
    {
        setNeedKeyFocus(true);  
    }

    void SpellListWidget::registerComponents()
    {
        MyGUI::FactoryManager::getInstance().registerFactory<SpellListWidget>("Widget");
    }

    void SpellListWidget::initialiseOverride()
    {
        assignWidget(mName, "Name"); 
        assignWidget(mSchool, "School");
        assignWidget(mCostChance, "CostChance");
        assignWidget(mItem, "Item");     
        Base::initialiseOverride();
    }

    void SpellListWidget::setStateFocused(bool focus)
    {
        if (focus)
        {
            MWBase::Environment::get().getWindowManager()->playSound("Inventory Hover");
            MWBase::Environment::get().getWindowManager()->setKeyFocusWidget(this);
            mName->setStateSelected(true);
            mOverlay->setVisible(true);
        }
        else 
        {
            MWBase::Environment::get().getWindowManager()->setKeyFocusWidget(nullptr);
            mName->setStateSelected(false);
            mOverlay->setVisible(false);
        }
    }

    void SpellListWidget::onMouseSetFocus(MyGUI::Widget *_old) 
    {
        MWBase::Environment::get().getWindowManager()->setKeyTooltip(true);
        setStateFocused(true);
        Base::onMouseSetFocus(_old);
        eventItemFocused(this);
    }

    void SpellListWidget::onMouseLostFocus(MyGUI::Widget *_new)
    {
        MWBase::Environment::get().getWindowManager()->setKeyTooltip(false);
        setStateFocused(false);
        Base::onMouseLostFocus(_new);
    }

}