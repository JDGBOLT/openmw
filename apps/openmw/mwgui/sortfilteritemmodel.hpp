#ifndef MWGUI_SORT_FILTER_ITEM_MODEL_H
#define MWGUI_SORT_FILTER_ITEM_MODEL_H

#include "itemmodel.hpp"

namespace MWGui
{
    class SortFilterItemModel : public ProxyItemModel
    {
    public:
        SortFilterItemModel (ItemModel* sourceModel);

        virtual void update();

        bool filterAccepts (const ItemStack& item);

        bool allowedToUseItems() const;
        virtual ItemStack getItem (ModelIndex index);
        virtual size_t getItemCount();

        /// Dragged items are not displayed.
        void addDragItem (const MWWorld::Ptr& dragItem, size_t count);
        void clearDragItems();
        
        void toggleSort(int sort);
        void setSort(int sort);
        int getSort() const;

        void setCategory (int category);
        void setFilter (int filter);
        void setNameFilter (const std::string& filter);
        void setEffectFilter (const std::string& filter);

        int getCategory() const;

        /// Use ItemStack::Type for sorting?
        void setSortByType(bool sort) { mSortByType = sort; }

        void onClose();
        bool onDropItem(const MWWorld::Ptr &item, int count);
        bool onTakeItem(const MWWorld::Ptr &item, int count);

        void updateSort(); 

        static const int Category_Weapon = (1<<1);
        static const int Category_Apparel = (1<<2);
        static const int Category_Misc = (1<<3);
        static const int Category_Magic = (1<<4);
        static const int Category_Armor = (1<<5);
        static const int Category_Cloth = (1<<6);
        static const int Category_Potion = (1<<7);
        static const int Category_Ingredient = (1<<8);
        static const int Category_Tool = (1<<9);
        static const int Category_Book = (1<<10);
        static const int Category_Simple = (1<<11);

        static const int Category_All = (1<<21) - 1;

        static const int Filter_OnlyIngredients = (1<<0);
        static const int Filter_OnlyEnchanted = (1<<1);
        static const int Filter_OnlyEnchantable = (1<<2);
        static const int Filter_OnlyChargedSoulstones = (1<<3);
        static const int Filter_OnlyUsableItems = (1<<4); // Only items with a Use action
        static const int Filter_OnlyRepairable = (1<<5);
        static const int Filter_OnlyRechargable = (1<<6);
        static const int Filter_OnlyRepairTools = (1<<7);

        static const int Sort_Name = (1<<0); 
        static const int Sort_Value = (1<<1); 
        static const int Sort_Weight = (1<<2);
        static const int Sort_Ratio = (1<<3);
        static const int Sort_WeaponType = (1<<4);
        static const int Sort_ArmorType = (1<<5);
        static const int Sort_Default = (1<<6)-1; 

    private:
        std::vector<ItemStack> mItems;

        std::vector<std::pair<MWWorld::Ptr, size_t> > mDragItems;

        int mCategory;
        int mFilter;
        int mSort;
        bool mIncreasing;
        bool mSortByType;

        std::string mNameFilter; // filter by item name
        std::string mEffectFilter; // filter by magic effect
    };
}

#endif
