#ifndef OPENMW_GUI_SPELLMODEL_H
#define OPENMW_GUI_SPELLMODEL_H

#include <memory>

#include "../mwworld/ptr.hpp"

namespace MWGui
{

    struct Spell
    {
        enum Type
        {
            Type_Power,
            Type_Spell,
            Type_EnchantedItem
        };

        Type mType;
        std::string mName;
        std::string mCostColumn; // Cost/chance or Cost/charge
        std::string mId; // Item ID or spell ID
        MWWorld::Ptr mItem; // Only for Type_EnchantedItem
        int mCount; // Only for Type_EnchantedItem
        bool mSelected; // Is this the currently selected spell/item (only one can be selected at a time)
        bool mActive; // (Items only) is the item equipped?

        Spell()
            : mType(Type_Spell)
            , mCount(0)
            , mSelected(false)
            , mActive(false)
        {
        }
    };

    ///@brief Model that lists all usable powers, spells and enchanted items for an actor.
    class SpellModel
    {
    public:
        SpellModel(const MWWorld::Ptr& actor, const std::string& filter);
        SpellModel(const MWWorld::Ptr& actor);

        typedef int ModelIndex;

        void update();

        Spell getItem (ModelIndex index) const;
        ///< throws for invalid index

        void setEffectFilter(const std::string filter);

        void setNameFilter(const std::string& filter)
        {
            mFilter = filter;
        }

        void setCategory(int category)
        {
            mCategory = category;
        }

        int getCategory() const 
        {
            return mCategory; 
        }

        void setSort(int sort)
        {
            mSort = sort;
        }

        void toggleSort(int sort)
        {
            mIncreasing = !mIncreasing; 
            mSort = sort; 
        }

        int getSort() const 
        {
            return mSort;
        }

        static const int Sort_Name = (1<<1);
        static const int Sort_School = (1<<2);
        static const int Sort_CostChance = (1<<3);
        
        // important, this matches the school IDs, do not change!
        static const int Category_Alteration = 0;
        static const int Category_Conjuration = 1;
        static const int Category_Destruction = 2;
        static const int Category_Illusion = 3;
        static const int Category_Mysticism = 4;
        static const int Category_Restoration = 5;
        static const int Category_Items = 6;
        static const int Category_Powers = 7;
        static const int Category_All = 8;
        static const int Category_Simple = 9;

        size_t getItemCount() const;
        ModelIndex getSelectedIndex() const;
        ///< returns -1 if nothing is selected

    private:
        MWWorld::Ptr mActor;

        std::vector<Spell> mSpells;

        std::string mFilter;
        std::string mEffectFilter;

        int mSort;
        int mCategory;
        bool mIncreasing;
    };
}

#endif
