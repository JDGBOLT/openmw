#ifndef OPENMW_MWGUI_INVENTORYWINDOW_H
#define OPENMW_MWGUI_INVENTORYWINDOW_H

#include "windowpinnablebase.hpp"
#include "mode.hpp"

#include <components/widgets/imagepushbutton.hpp>
#include <components/widgets/box.hpp>

#include "itemlistwidget.hpp"

#include "../mwworld/ptr.hpp"
#include "../mwrender/characterpreview.hpp"

namespace osg
{
    class Group;
}

namespace Resource
{
    class ResourceSystem;
}

namespace MWGui
{
    namespace Widgets
    {
        class MWDynamicStat;
    }

    class ItemView;
    class SortFilterItemModel;
    class TradeItemModel;
    class DragAndDrop;
    class ItemModel;

    class InventoryWindow : public WindowPinnableBase
    {
        public:

            enum Mode {
                View_Item,
                View_Avatar
            };

            InventoryWindow(DragAndDrop* dragAndDrop, osg::Group* parent, Resource::ResourceSystem* resourceSystem);

            virtual void onOpen();

            /// start trading, disables item drag&drop
            void setTrading(bool trading);

            void onFrame(float dt);

            void pickUpObject (MWWorld::Ptr object);

            MWWorld::Ptr getAvatarSelectedItem(int x, int y);
            
            void rebuildAvatar();

            SortFilterItemModel* getSortFilterModel();
            TradeItemModel* getTradeModel();
            ItemModel* getModel();

            void updateItemView();

            void updatePlayer();

            void adjustCategoryHeader();

            void clear();

            void useItem(const MWWorld::Ptr& ptr, bool force=false);

            void setGuiMode(GuiMode mode);

            void resetAvatar();

            /// Cycle to previous/next weapon
            void cycle(bool next);

        protected:
            virtual void onTitleDoubleClicked();

        private:
            DragAndDrop* mDragAndDrop;

            int mSelectedItem;

            MWWorld::Ptr mPtr;

            MWGui::ItemView* mItemView;
            SortFilterItemModel* mSortModel;
            TradeItemModel* mTradeModel;

            MyGUI::TextBox* mPlayerGold; 
            MyGUI::TextBox* mArmorRating;
            MyGUI::TextBox* mEncumbranceBar;

            MyGUI::EditBox* mDescription;

            Gui::ImagePushButton* mAllButton;
            Gui::ImagePushButton* mWeaponButton;
            Gui::ImagePushButton* mArmorButton;
            Gui::ImagePushButton* mClothButton;
            Gui::ImagePushButton* mPotionButton;
            Gui::ImagePushButton* mIngredientButton;
            Gui::ImagePushButton* mBookButton;
            Gui::ImagePushButton* mToolButton;
            Gui::ImagePushButton* mMagicButton;
            Gui::ImagePushButton* mMiscButton;
            Gui::ImagePushButton* mToggleAvatar;

            MyGUI::Widget* mCategories;
            
            MyGUI::EditBox* mFilterEdit;

            GuiMode mGuiMode;

            int mLastXSize;
            int mLastYSize;

            int mRoll;
            int mPitch;
            int mYaw;

            bool mTrading;
            bool mDrop; 
            float mScaleFactor;
            float mUpdateTimer; 

            Mode mViewMode;

            double mScale;

            MyGUI::Widget* mAvatar;
            MyGUI::ImageBox* mAvatarImage;

            MyGUI::Widget* mLeftPane;
            MyGUI::Widget* mRightPane;

            std::unique_ptr<MyGUI::ITexture> mPreviewTexture;
            std::unique_ptr<MWRender::InventoryPreview> mPreview;

            MyGUI::IntPoint mLastDragPos;

            void toggleMaximized();
            
            void onHeaderClicked(int sort);
            void onToggleItem(MyGUI::Widget* sender, int count);
            void onKeyButtonPressed(MyGUI::Widget* sender, MyGUI::KeyCode key);
            void onItemFocus(ItemListWidget* item);
            void onItemSelected(int index);
            void onItemSelectedFromSourceModel(int index);
            void onMouseWheel(MyGUI::Widget* _sender, int _rel);

            void onDragStart(MyGUI::Widget* _sender, int _left, int _top, MyGUI::MouseButton _id);
            void onMouseDrag(MyGUI::Widget* _sender, int _left, int _top, MyGUI::MouseButton _id);

            void onBackgroundSelected();

            std::string getModeSetting() const;

            void sellItem(MyGUI::Widget* sender, int count);
            void dragItem(MyGUI::Widget* sender, int count);

            void onWindowResize(MyGUI::Window* _sender);
            void onFilterChanged(MyGUI::Widget* _sender);
            void onNameFilterChanged(MyGUI::EditBox* _sender);
            void onAvatarClicked(MyGUI::Widget* _sender);
            void onAvatarToggled(MyGUI::Widget* _sender);
            void onPinToggled();

            void notifyContentChanged();
            void dirtyPreview();
            void updateEncumbranceBar();
            void updatePreviewSize();
            void updateArmorRating();
            void updatePlayerGold();

            void adjustPanes();

            /// Unequips count items from mSelectedItem, if it is equipped, and then updates mSelectedItem in case the items were re-stacked
            void ensureSelectedItemUnequipped(int count);
    };
}

#endif // Inventory_H
