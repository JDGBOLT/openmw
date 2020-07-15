#ifndef MWGUI_QUICKLOOT_H
#define MWGUI_QUICKLOOT_H

#include "layout.hpp"
#include "../mwworld/ptr.hpp"

#include "widgets.hpp"
#include "itemview.hpp"
#include "itemmodel.hpp"
#include "sortfilteritemmodel.hpp"

namespace MWGui
{
    class QuickLoot : public Layout
    {
    public:
        QuickLoot();

        void onFrame(float frameDuration);
        void update(float frameDuration);

        void setEnabled(bool enabled);

        void setDelay(float delay);

        bool isVisible() const { return mMainWidget->getVisible() && mQuickLoot->getVisible(); }

        bool isPlaying() const { return mPlaying;}
        void setPlaying(bool playing) { mPlaying = false; }

        void notifyMouseWheel(int rel);

        void clear();

        void setFocusObject(const MWWorld::Ptr& focus);
        void setFocusObjectScreenCoords(float min_x, float min_y, float max_x, float max_y);
        ///< set the screen-space position of the tooltip for focused object
        
        bool checkOwned();

        void resize();

        void ensureTrapTriggered(); 

    private:

        void playOpenAnimation();
        void playCloseAnimation() const;

        void setVisibleAll(bool visible);

        ItemView* mQuickLoot;
        ItemModel* mModel;
        MyGUI::TextBox* mLabel;
        SortFilterItemModel* mSortModel;

        /// has the current container been "opened"
        bool mOpened;
        bool mShouldOpen;

        bool mHidden;
        bool mPlaying;

        MWWorld::Ptr mFocusObject;
        MWWorld::Ptr mLastFocusObject;

        float mFocusToolTipX;
        float mFocusToolTipY;

        /// Adjust position for a tooltip so that it doesn't leave the screen and does not obscure the mouse cursor
        void position(MyGUI::IntPoint& position, MyGUI::IntSize size, MyGUI::IntSize viewportSize);

        void onKeyButtonPressed(MyGUI::Widget* sender, MyGUI::KeyCode key);
        void onItemSelected(int index);

        int mHorizontalScrollIndex;

        float mDelay;
        float mRemainingDelay; // remaining time until tooltip will show

        int mLastMouseX;
        int mLastMouseY;

        bool mEnabled;
        
        float mFrameDuration;

        int mLastIndex;
    };
}
#endif
