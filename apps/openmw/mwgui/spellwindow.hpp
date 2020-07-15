#ifndef MWGUI_SPELLWINDOW_H
#define MWGUI_SPELLWINDOW_H

#include <MyGUI_ComboBox.h>

#include "windowpinnablebase.hpp"

#include <components/widgets/imagepushbutton.hpp>

#include "../mwworld/ptr.hpp"

#include "spellmodel.hpp"

namespace MWGui
{
    class SpellIcons;
    class SpellView;

    class SpellWindow : public WindowPinnableBase, public NoDrop
    {
    public:
        SpellWindow(DragAndDrop* drag);
        virtual ~SpellWindow();

        void updateFilterEffect();

        void updateSpells();

        void onFrame(float dt);

        /// Cycle to next/previous spell
        void cycle(bool next);

        void adjustCategoryHeader();

        void applyFilter(const std::string& filter);

    protected:
        MyGUI::Widget* mEffectBox;

        std::string mSpellToDelete;

        void onHeaderClicked(int sort);
        void onEnchantedItemSelected(MWWorld::Ptr item, bool alreadyEquipped);
        void onSpellSelected(const std::string& spellId);
        void onModelIndexSelected(SpellModel::ModelIndex index);
        void onNameFilterChanged(MyGUI::EditBox *sender);
        void onDeleteClicked(MyGUI::Widget *widget);
        void onDeleteSpellAccept();
        void askDeleteSpell(const std::string& spellId);
        void onCategoryFilterChanged(MyGUI::Widget* _sender);

        void onFilterChanged(MyGUI::ComboBox* _sender, size_t _index);
        void onFilterEdited(MyGUI::EditBox* _sender);

        virtual void onPinToggled();
        virtual void onTitleDoubleClicked();
        virtual void onOpen();

        SpellView* mSpellView;
        SpellIcons* mSpellIcons;

        SpellModel *mSortModel;
        
        MyGUI::ComboBox* mFilterValue;
        MyGUI::EditBox* mFilterEdit;

        Gui::ImagePushButton* mAllButton;
        Gui::ImagePushButton* mPowersButton;
        Gui::ImagePushButton* mItemsButton;
        Gui::ImagePushButton* mAlterationButton;
        Gui::ImagePushButton* mConjurationButton;
        Gui::ImagePushButton* mDestructionButton;
        Gui::ImagePushButton* mIllusionButton;
        Gui::ImagePushButton* mMysticismButton;
        Gui::ImagePushButton* mRestorationButton;

        MyGUI::Widget* mCategories;

    private:
        float mUpdateTimer;

    };
}

#endif
