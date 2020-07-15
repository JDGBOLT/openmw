#include "itemview.hpp"

#include <cmath>

#include <MyGUI_FactoryManager.h>
#include <MyGUI_Gui.h>
#include <MyGUI_ImageBox.h>
#include <MyGUI_TextBox.h>
#include <MyGUI_ScrollView.h>
#include <MyGUI_Button.h>

#include "../mwworld/class.hpp"
#include "../mwworld/inventorystore.hpp"

#include "../mwbase/environment.hpp"
#include "../mwbase/inputmanager.hpp"
#include "../mwbase/windowmanager.hpp"

#include "../mwmechanics/actorutil.hpp"

#include "../mwinput/inputmanagerimp.hpp"
#include "../mwinput/sdlmappings.hpp"

#include "itemmodel.hpp"
#include "quickloot.hpp"
#include "itemwidget.hpp"
#include "itemlistwidget.hpp"
#include "inventorywindow.hpp"
#include "itemlistwidgetheader.hpp"

namespace MWGui
{

ItemView::ItemView()
    : mModel(nullptr)
    , mLastIndex(0)
    , mScrollView(nullptr)
    , mHeader(nullptr)
    , mDisableHeader(false)
{
}

ItemView::~ItemView()
{
    delete mModel;
}

void ItemView::setModel(ItemModel *model)
{
    if (mModel == model)
        return;

    delete mModel;
    mModel = model;

    update(true);
}

void ItemView::initialiseOverride()
{
    Base::initialiseOverride();

    assignWidget(mScrollView, "ScrollView");
    if (mScrollView == nullptr)
        throw std::runtime_error("Item view needs a scroll view");
    assignWidget(mHeader, "Header"); 
    if (mHeader == nullptr)
        throw std::runtime_error("Item view needs a header view");
    mScrollView->setCanvasAlign(MyGUI::Align::Left | MyGUI::Align::Top);
}

void ItemView::layoutWidgets()
{
    if (!mScrollView->getChildCount())
        return;

    MyGUI::Widget* dragArea = mScrollView->getChildAt(0);
    int y = 0;

    int height = 0;
    for (size_t i = 0; i < dragArea->getChildCount(); i++)
        height += dragArea->getChildAt(i)->getHeight();

    bool scrollVisible = height > mScrollView->getHeight();
    int rightMargin = (scrollVisible) ? 45 :10;

    if (!mDisableHeader)
        mHeader->setSize(MyGUI::IntSize(mScrollView->getWidth()-rightMargin,36));

    unsigned int count = dragArea->getChildCount();
    int h = (count) ? dragArea->getChildAt(0)->getHeight() : 0; 

    for (unsigned int i=0; i<count; ++i)
    {
        dragArea->getChildAt(i)->setCoord(MyGUI::IntCoord(
            0,y, dragArea->getWidth()-rightMargin, dragArea->getChildAt(0)->getHeight()));
        y += h;
    }

    MyGUI::IntSize size = MyGUI::IntSize(mScrollView->getSize().width,std::max(mScrollView->getSize().height, y));

    // Canvas size must be expressed with VScroll disabled, otherwise MyGUI would expand the scroll area when the scrollbar is hidden
    mScrollView->setVisibleVScroll(false);
    mScrollView->setVisibleHScroll(false);
    mScrollView->setCanvasSize(size);
    mScrollView->setVisibleVScroll(true);
    mScrollView->setVisibleHScroll(true);
    dragArea->setSize(size);
}

void ItemView::update(bool force)
{
    if (!mModel)
        return;

    mModel->update();
    
    // updating is costly here (TODO: optimize by using a multilistbox maybe?)
    // note: update() itself is not causing bad performance with huge inventory size when 
    // resizing, moving windows, and (less pronounced) when sliding with mouse. 
    // Instead, it seems to be with redrawing the necassary widgets. 
    
    
    if (!force && mScrollView->getChildCount() > 0 
            && mScrollView->getChildAt(0)->getChildCount() == mModel->getItemCount())
    {
        bool needsUpdate = false;
        for (size_t i = 0; i < mModel->getItemCount(); i++)
        {
            auto w = dynamic_cast<ItemListWidget*>(mScrollView->getChildAt(0)->getChildAt(i));
            if (w == nullptr || mModel->getItem(i) != w->getItemStack()
            || mModel->getItem(i).mCount != w->getItemStack().mCount)
            {
                needsUpdate = true;
                break;
            }
        }
        if (!needsUpdate)  return;
    }

    while (mScrollView->getChildCount())
        MyGUI::Gui::getInstance().destroyWidget(mScrollView->getChildAt(0));
    while (mHeader->getChildCount())
        MyGUI::Gui::getInstance().destroyWidget(mHeader->getChildAt(0)); 

    MyGUI::Widget* dragArea = mScrollView->createWidget<MyGUI::Widget>("",0,0,mScrollView->getWidth(),mScrollView->getHeight(),
                                                                       MyGUI::Align::Stretch);
    dragArea->setNeedMouseFocus(true);
    dragArea->eventMouseButtonClick += MyGUI::newDelegate(this, &ItemView::onSelectedBackground);
    dragArea->eventMouseWheel += MyGUI::newDelegate(this, &ItemView::onMouseWheelMoved);
    
    int category = dynamic_cast<MWGui::SortFilterItemModel*>(mModel)->getCategory();  
    
    if (!mDisableHeader)
    {

        if (category == MWGui::SortFilterItemModel::Category_Weapon)
            mHeader->changeWidgetSkin("MW_ItemListHeader_Weapon");
        else if (category == MWGui::SortFilterItemModel::Category_Armor)
            mHeader->changeWidgetSkin("MW_ItemListHeader_Armor");
        else if (category == MWGui::SortFilterItemModel::Category_Simple)
            mHeader->changeWidgetSkin("MW_ItemListHeader_Simple");
        else 
            mHeader->changeWidgetSkin("MW_ItemListHeader_All");
    }
    else 
    {
        mHeader->setVisible(false);
        mHeader->setSize(0,0);
    }

    for (ItemModel::ModelIndex i=0; i < static_cast<int>(mModel->getItemCount()); ++i)
    {
        const ItemStack& item = mModel->getItem(i);
        ItemListWidget* itemWidget = dragArea->createWidget<ItemListWidget>("MW_ItemList",
            MyGUI::IntCoord(0,0,mScrollView->getWidth(), 35), MyGUI::Align::HStretch | MyGUI::Align::Top);

        itemWidget->setNeedKeyFocus(true);
        itemWidget->setNeedMouseFocus(true);
        itemWidget->setItem(item, category);
        itemWidget->setUserString("ToolTipType", "ItemModelIndex");
        itemWidget->setUserData(std::make_pair(i,mModel)); 
        itemWidget->eventMouseButtonClick += MyGUI::newDelegate(this, &ItemView::onSelectedItem);
        itemWidget->eventMouseWheel += MyGUI::newDelegate(this, &ItemView::onMouseWheelMoved);
        itemWidget->eventKeyButtonPressed += MyGUI::newDelegate(this, &ItemView::onKeyButtonPressed);
        itemWidget->eventItemFocused += MyGUI::newDelegate(this, &ItemView::onItemFocused);
    }
    layoutWidgets();
}

int ItemView::forceItemFocused(int index)
{
    if (!mModel)
        return 0;

    if (!mScrollView->getChildCount())
        return 0;
    int count = mScrollView->getChildAt(0)->getChildCount();
    if (!count )
        return 0;
    // handle last element specially 
    if (index > count - 1)
        index = count -1;

    auto w = dynamic_cast<ItemListWidget*>(mScrollView->getChildAt(0)->getChildAt(index));
    if (w)
    {
        w->setStateFocused(true);
        onItemFocused(w);
    }

    // make sure we adjust for scrolling, this is just an approximation
    if (mScrollView->isVisibleVScroll())
    {
        // how many items the scrollview can show
        int maxItems = mScrollView->getHeight() / w->getHeight();
        int minIndex = std::ceil((-mScrollView->getViewOffset().top / static_cast<float>(w->getHeight())));
        int maxIndex = minIndex + maxItems - 1;

        // this is *not* a scroll-to, it assumes the item is already in view
        if (index > maxIndex)
        {
            mScrollView->setViewOffset(MyGUI::IntPoint(0, static_cast<int>(mScrollView->getViewOffset().top - w->getHeight())));
        }
        else if (index < minIndex)
        {
            mScrollView->setViewOffset(MyGUI::IntPoint(0, static_cast<int>(mScrollView->getViewOffset().top + w->getHeight())));
        }
    }

    return index;
}

int ItemView::requestListSize() const
{
    if (!mScrollView->getChildCount())
        return 0;
    int count = mScrollView->getChildAt(0)->getChildCount();
    if (!count )
        return 0;

    return count * mScrollView->getChildAt(0)->getChildAt(0)->getHeight();
}

void ItemView::onItemFocused(ItemListWidget* item)
{
    for (size_t i = 0; i < mScrollView->getChildAt(0)->getChildCount();i++)
    {
        auto w = dynamic_cast<ItemListWidget*>(mScrollView->getChildAt(0)->getChildAt(i));
        if (item != w)
            w->setStateFocused(false);
        else 
            mLastIndex = i;
    }
    MWBase::Environment::get().getWindowManager()->setKeyFocusWidget(item);
    eventItemFocused(item);
}

void ItemView::resetScrollBars()
{
    mScrollView->setViewOffset(MyGUI::IntPoint(0, 0));
}

void ItemView::onSelectedItem(MyGUI::Widget *sender)
{
    ItemModel::ModelIndex index = (*sender->getUserData<std::pair<ItemModel::ModelIndex, ItemModel*> >()).first;
    mLastIndex = index;
    eventItemClicked(index);
}

void ItemView::onKeyButtonPressed(MyGUI::Widget *sender, MyGUI::KeyCode key, MyGUI::Char character)
{
    ItemModel::ModelIndex index = (*sender->getUserData<std::pair<ItemModel::ModelIndex, ItemModel*> >()).first;

    if (key == MyGUI::KeyCode::ArrowUp || key == MyGUI::KeyCode::ArrowDown)
    {
        if (mScrollView->getChildCount() > 0 && mScrollView->getChildAt(0)->getChildCount() > 0)
        {
            int count = mScrollView->getChildAt(0)->getChildCount();
            for (auto i = 0; i < count; i++)
            {
                ItemListWidget* w = dynamic_cast<ItemListWidget*>(mScrollView->getChildAt(0)->getChildAt(i));
                w->setStateFocused(false);
            }

            if (index < 0 || index > count - 1)
                index = 0;

            if (key == MyGUI::KeyCode::ArrowUp)
            {
                if (index < 2)
                    index = 0;
                else 
                    index -= 1;
            }
            else
            {
                if (index >= count - 2)
                    index = count - 1;
                else 
                    index += 1;
            }

            ItemListWidget* w = dynamic_cast<ItemListWidget*>(mScrollView->getChildAt(0)->getChildAt(index));
            w->setStateFocused(true);
            onItemFocused(w);

            // make sure we adjust for scrolling, this is just an approximation 
            if (mScrollView->isVisibleVScroll())
            {
                // how many items the scrollview can show 
                int maxItems = mScrollView->getHeight() / w->getHeight();
                int minIndex = std::ceil((-mScrollView->getViewOffset().top / static_cast<float>(w->getHeight())));
                int maxIndex = minIndex + maxItems - 1;

                // this is *not* a scroll-to, it assumes the item is already in view 
                if (index > maxIndex)
                {
                    mScrollView->setViewOffset(MyGUI::IntPoint(0,static_cast<int>(mScrollView->getViewOffset().top - w->getHeight())));
                }
                else if (index < minIndex)
                {
                    mScrollView->setViewOffset(MyGUI::IntPoint(0,static_cast<int>(mScrollView->getViewOffset().top + w->getHeight())));
                }
            }
        }
    }
    else if (MWBase::Environment::get().getWindowManager()->getQuickLoot()->isVisible())
    {
        SDL_Keycode quickkey = SDL_GetKeyFromName(Settings::Manager::getString("key quickloot take", "MorroUI").c_str());
        MyGUI::KeyCode kc = MWInput::sdlKeyToMyGUI(quickkey);

        if (static_cast<int>(key.getValue()) == static_cast<int>(kc.getValue()))
            eventItemClicked(index);
    }
    else if (key == MyGUI::KeyCode::Return)
        eventItemClicked(index);

    eventKeyButtonPressed(sender, key);
}

void ItemView::onSelectedBackground(MyGUI::Widget *sender)
{
    eventBackgroundClicked();
}

void ItemView::onMouseWheelMoved(MyGUI::Widget *_sender, int _rel)
{
    if (mScrollView->getViewOffset().top + _rel*0.3f > 0)
        mScrollView->setViewOffset(MyGUI::IntPoint(0, 0));
    else
        mScrollView->setViewOffset(MyGUI::IntPoint(0,static_cast<int>(mScrollView->getViewOffset().top + _rel*0.3f)));
}

void ItemView::setSize(const MyGUI::IntSize &_value)
{
    bool changed = (_value.width != getWidth() || _value.height != getHeight());
    Base::setSize(_value);
    if (changed)
        layoutWidgets();
}

void ItemView::setCoord(const MyGUI::IntCoord &_value)
{
    bool changed = (_value.width != getWidth() || _value.height != getHeight());
    Base::setCoord(_value);
    if (changed)
        layoutWidgets();
}

void ItemView::registerComponents()
{
    MyGUI::FactoryManager::getInstance().registerFactory<MWGui::ItemView>("Widget");
}

}
