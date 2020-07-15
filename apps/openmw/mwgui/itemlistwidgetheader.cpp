#include "itemlistwidgetheader.hpp"

#include <MyGUI_Gui.h>
#include <MyGUI_FactoryManager.h>
#include <MyGUI_RenderManager.h>
#include <MyGUI_Button.h>

#include "../mwbase/windowmanager.hpp"
#include "../mwbase/environment.hpp"

#include "inventorywindow.hpp"
#include "spellmodel.hpp"
#include "sortfilteritemmodel.hpp"

namespace MWGui
{
    ItemListWidgetHeader::ItemListWidgetHeader()
        : mName(nullptr)
        , mValue(nullptr)
        , mWeight(nullptr)
        , mRatio(nullptr)
        , mArmorClass(nullptr)
        , mWeaponType(nullptr)
        , mSpellName(nullptr)
        , mCostChance(nullptr)
        , mSchool(nullptr)
    {

    }

    void ItemListWidgetHeader::registerComponents()
    {
        MyGUI::FactoryManager::getInstance().registerFactory<ItemListWidgetHeader>("Widget");
    }

    void ItemListWidgetHeader::initialiseOverride()
    {
        assignWidget(mName, "Name");
        assignWidget(mSpellName, "SpellName");
        assignWidget(mValue, "Value");
        assignWidget(mWeight, "Weight");
        assignWidget(mRatio, "Ratio");
        assignWidget(mArmorClass, "ArmorClass");
        assignWidget(mWeaponType, "WeaponType");
        assignWidget(mCostChance, "CostChance");
        assignWidget(mSchool, "School");

        if (mName)
        {
            mName->setUserData(MWGui::SortFilterItemModel::Sort_Name);
            mName->eventMouseButtonClick += MyGUI::newDelegate(this, &ItemListWidgetHeader::onSortClicked);
        }
        if (mValue)
        {
            mValue->setUserData(MWGui::SortFilterItemModel::Sort_Value);
            mValue->eventMouseButtonClick += MyGUI::newDelegate(this, &ItemListWidgetHeader::onSortClicked);
        }
        if (mWeight)
        {
            mWeight->setUserData(MWGui::SortFilterItemModel::Sort_Weight);
            mWeight->eventMouseButtonClick += MyGUI::newDelegate(this, &ItemListWidgetHeader::onSortClicked);
        }
        if (mRatio)
        {
            mRatio->setUserData(MWGui::SortFilterItemModel::Sort_Ratio);
            mRatio->eventMouseButtonClick += MyGUI::newDelegate(this, &ItemListWidgetHeader::onSortClicked);
        }
        if (mArmorClass)
        {
            mArmorClass->setUserData(MWGui::SortFilterItemModel::Sort_ArmorType);
            mArmorClass->eventMouseButtonClick += MyGUI::newDelegate(this, &ItemListWidgetHeader::onSortClicked);
        }
        if (mWeaponType)
        {
            mWeaponType->setUserData(MWGui::SortFilterItemModel::Sort_WeaponType);
            mWeaponType->eventMouseButtonClick += MyGUI::newDelegate(this, &ItemListWidgetHeader::onSortClicked);
        }
        if (mSpellName)
        {
            mSpellName->setUserData(MWGui::SpellModel::Sort_Name);
            mSpellName->eventMouseButtonClick += MyGUI::newDelegate(this, &ItemListWidgetHeader::onSortClicked);
        }
        // TODO 
        //if (mCostChance)
        //{
        //    mCostChance->setUserData(MWGui::SpellModel::Sort_CostChance);
        //    mCostChance->eventMouseButtonClick += MyGUI::newDelegate(this, &ItemListWidgetHeader::onSortClicked);
        //}
        if (mSchool)
        {
            mSchool->setUserData(MWGui::SpellModel::Sort_School);
            mSchool->eventMouseButtonClick += MyGUI::newDelegate(this, &ItemListWidgetHeader::onSortClicked);
        }

        Base::initialiseOverride();
    }

    void ItemListWidgetHeader::onSortClicked(MyGUI::Widget* sender)
    {
        if (sender == nullptr) return;
        
        eventItemClicked(*sender->getUserData<int>());
    }
}