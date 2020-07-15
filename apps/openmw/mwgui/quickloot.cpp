#include "quickloot.hpp"

#include <MyGUI_Gui.h>
#include <MyGUI_RenderManager.h>
#include <MyGUI_InputManager.h>
#include <MyGUI_ImageBox.h>

#include <components/settings/settings.hpp>
#include <components/widgets/box.hpp>

#include "../mwbase/world.hpp"
#include "../mwbase/environment.hpp"
#include "../mwbase/soundmanager.hpp"
#include "../mwbase/windowmanager.hpp"
#include "../mwbase/mechanicsmanager.hpp"

#include "../mwworld/class.hpp"
#include "../mwworld/action.hpp"
#include "../mwworld/player.hpp"
#include "../mwworld/esmstore.hpp"
#include "../mwworld/actionopen.hpp"
#include "../mwworld/actiontake.hpp"
#include "../mwworld/actiontrap.hpp"
#include "../mwworld/inventorystore.hpp"

#include "../mwmechanics/actor.hpp"
#include "../mwmechanics/disease.hpp"
#include "../mwmechanics/npcstats.hpp"
#include "../mwmechanics/character.hpp"
#include "../mwmechanics/actorutil.hpp"
#include "../mwmechanics/creaturestats.hpp"

#include "../mwinput/inputmanagerimp.hpp"
#include "../mwinput/sdlmappings.hpp"

#include "../mwrender/animation.hpp"

#include "inventorywindow.hpp"
#include "containeritemmodel.hpp"
#include "inventoryitemmodel.hpp"
#include "pickpocketitemmodel.hpp"

#include "itemmodel.hpp"

namespace MWGui
{
    QuickLoot::QuickLoot() :
        Layout("openmw_quickloot.layout")
        , mFocusToolTipX(0.0)
        , mFocusToolTipY(0.0)
        , mHorizontalScrollIndex(0)
        , mDelay(0.0)
        , mRemainingDelay(0.0)
        , mLastMouseX(0)
        , mLastMouseY(0)
        , mEnabled(true)
        , mFrameDuration(0.f)
        , mQuickLoot(nullptr)
        , mModel(nullptr)
        , mSortModel(nullptr)
        , mLastIndex(0)
        , mPlaying(false)
    {
        getWidget(mQuickLoot, "QuickLoot");
        getWidget(mLabel, "Label");

        setVisibleAll(false);

        mQuickLoot->eventKeyButtonPressed += MyGUI::newDelegate(this, &QuickLoot::onKeyButtonPressed);
        mQuickLoot->eventItemClicked += MyGUI::newDelegate(this, &QuickLoot::onItemSelected);

        mQuickLoot->disableHeader(true);

        // turn off mouse focus so that getMouseFocusWidget returns the correct widget,
        // even if the mouse is over the tooltip
        mQuickLoot->setNeedKeyFocus(true);
        mQuickLoot->setNeedMouseFocus(false);
        mMainWidget->setNeedMouseFocus(false);
        mMainWidget->setNeedKeyFocus(false);

        // same delay as tooltips, for now 
        mDelay = Settings::Manager::getFloat("tooltip delay", "GUI");
        mRemainingDelay = mDelay;
    }

    void QuickLoot::notifyMouseWheel(int rel)
    {
        if (rel < 0)
        {
            mLastIndex++;
            if (mSortModel && mLastIndex >= mSortModel->getItemCount())
                mLastIndex = mSortModel->getItemCount()-1;
        }
        else 
            mLastIndex = std::max(0, mLastIndex - 1);

        mQuickLoot->forceItemFocused(mLastIndex);
        mQuickLoot->update();
    }

    bool QuickLoot::checkOwned()
    {
        if(mFocusObject.isEmpty())
            return false;

        MWWorld::Ptr ptr = MWMechanics::getPlayer();
        MWWorld::Ptr victim;

        MWBase::MechanicsManager* mm = MWBase::Environment::get().getMechanicsManager();
        return !mm->isAllowedToUse(ptr, mFocusObject, victim);
    }


    void QuickLoot::onItemSelected(int index)
    {
        if (!mModel) 
            return;

        if (!MWBase::Environment::get().getWindowManager()->isAllowed(MWGui::GW_Inventory))
            return;
        
        mOpened = true;
        ensureTrapTriggered();
        MWMechanics::diseaseContact(MWMechanics::getPlayer(), mFocusObject);
        
        const ItemStack& item = mModel->getItem(mSortModel->mapToSource(index));

        MWWorld::Ptr object = item.mBase;
        int count = 1;
        bool shift = MyGUI::InputManager::getInstance().isShiftPressed();

        if (shift)
            count = item.mCount;

        if (!mModel->onTakeItem(item.mBase,count))
            return;

        ItemModel* playerModel = MWBase::Environment::get().getWindowManager()->getInventoryWindow()->getModel();
        mModel->update();
        auto movedItem = mModel->moveItem(item, count, playerModel);
        MWBase::Environment::get().getWindowManager()->getInventoryWindow()->updateItemView();

        if (MyGUI::InputManager::getInstance().isControlPressed())
            MWBase::Environment::get().getWindowManager()->getInventoryWindow()->useItem(movedItem);
        else 
        {
            std::string sound = item.mBase.getClass().getUpSoundId(item.mBase);
            MWBase::Environment::get().getWindowManager()->playSound(sound);
        }

        mQuickLoot->update();
        mQuickLoot->forceItemFocused(index);
    }

    void QuickLoot::ensureTrapTriggered()
    {
        if (mFocusObject.isEmpty() || mFocusObject.getTypeName() != typeid(ESM::Container).name()) 
            return;

        MWWorld::Ptr player = MWBase::Environment::get().getWorld()->getPlayerPtr();
        MWWorld::InventoryStore &invStore = player.getClass().getInventoryStore(player);

        bool isTrapped = !mFocusObject.getCellRef().getTrap().empty();
        bool hasKey = false;
        std::string keyName;

        static const std::string trapActivationSound = "Disarm Trap Fail";

        // necassary since having the key will always deactivate the trap
        const std::string keyId = mFocusObject.getCellRef().getKey();
        if (!keyId.empty())
        {
            MWWorld::Ptr keyPtr = invStore.search(keyId);
            if (!keyPtr.isEmpty())
            {
                hasKey = true;
                keyName = keyPtr.getClass().getName(keyPtr);
            }
        }

        if (isTrapped && hasKey)
        {
            MWBase::Environment::get().getWindowManager()->messageBox(keyName + " #{sKeyUsed}");
            // using a key disarms the trap
            if (isTrapped)
            {
                mFocusObject.getCellRef().setTrap("");
                MWBase::Environment::get().getSoundManager()->playSound3D(mFocusObject, "Disarm Trap", 1.0f, 1.0f);
            }
        }
        else if (isTrapped)
        {
            // Activate trap
            std::shared_ptr<MWWorld::Action> action(new MWWorld::ActionTrap(mFocusObject.getCellRef().getTrap(), mFocusObject));
            action->setSound(trapActivationSound);
            action->execute(player);
        }
    }

    void QuickLoot::onKeyButtonPressed(MyGUI::Widget* sender, MyGUI::KeyCode key)
    {
        SDL_Keycode quickkey = SDL_GetKeyFromName(Settings::Manager::getString("key quickloot takeall", "MorroUI").c_str());
        MyGUI::KeyCode kc = MWInput::sdlKeyToMyGUI(quickkey);

        if (static_cast<int>(key.getValue()) == static_cast<int>(kc.getValue())) // take all 
        {
            if (!mModel) return;

            mOpened = true;
            ensureTrapTriggered();
            MWMechanics::diseaseContact(MWMechanics::getPlayer(), mFocusObject);

            // transfer everything into the player's inventory
            ItemModel* playerModel = MWBase::Environment::get().getWindowManager()->getInventoryWindow()->getModel();
            mModel->update();

            // unequip all items to avoid unequipping/reequipping
            if (mFocusObject.getClass().hasInventoryStore(mFocusObject))
            {
                MWWorld::InventoryStore& invStore = mFocusObject.getClass().getInventoryStore(mFocusObject);
                for (size_t i=0; i < mModel->getItemCount(); ++i)
                {
                    const ItemStack& item = mModel->getItem(i);
                    if (invStore.isEquipped(item.mBase) == false)
                        continue;

                    invStore.unequipItem(item.mBase, mFocusObject);
                }
            }

            mModel->update();

            for (size_t i=0; i<mModel->getItemCount(); ++i)
            {
                if (i==0)
                {
                    // play the sound of the first object
                    MWWorld::Ptr item = mModel->getItem(i).mBase;
                    std::string sound = item.getClass().getUpSoundId(item);
                    MWBase::Environment::get().getWindowManager()->playSound(sound);
                }

                const ItemStack& item = mModel->getItem(i);

                if (!mModel->onTakeItem(item.mBase, item.mCount))
                    break;

                mModel->moveItem(item, item.mCount, playerModel);
            }
        }
    }

    void QuickLoot::setEnabled(bool enabled)
    {
        mEnabled = enabled;
    }

    void QuickLoot::onFrame(float frameDuration)
    {
        mFrameDuration = frameDuration;
    }

    void QuickLoot::setVisibleAll(bool visible)
    {
        mHidden = !visible;
        if (visible && mOpened && mShouldOpen)
        {
            playOpenAnimation();
            mShouldOpen = false;
        }

        if (visible)
            resize();

        mMainWidget->setVisible(visible);
        for (int i = 0; i < mMainWidget->getChildCount(); i++)
            mMainWidget->getChildAt(i)->setVisible(visible);
    }

    void QuickLoot::update(float frameDuration)
    {
        if (!mEnabled)
        {
            return;
        }

        MWBase::WindowManager *winMgr = MWBase::Environment::get().getWindowManager();
        bool guiMode = winMgr->isGuiMode();
        bool inCombat = MWBase::Environment::get().getWorld()->getPlayer().isInCombat();

        if (guiMode)
        {
            setVisibleAll(false);
            return;
        }

        
        if (!mFocusObject.isEmpty() && mFocusObject.getCellRef().getLockLevel() <= 0)
        {
            mQuickLoot->setModel(nullptr);
            mModel = nullptr;
            mSortModel = nullptr;
            mQuickLoot->update();

            bool loot = mFocusObject.getClass().isActor() && mFocusObject.getClass().getCreatureStats(mFocusObject).isDead();
            bool sneaking = MWBase::Environment::get().getMechanicsManager()->isSneaking(MWMechanics::getPlayer());

            auto skin = (checkOwned()) ?  "HUD_Box_Owned" : "HUD_Box";

            mQuickLoot->getParent()->changeWidgetSkin(skin);

            if (mFocusObject.getClass().hasInventoryStore(mFocusObject))
            {
                if (mFocusObject.getClass().isNpc() && !loot && sneaking && !inCombat)
                {
                    mModel = new PickpocketItemModel(mFocusObject, new InventoryItemModel(mFocusObject),
                        !mFocusObject.getClass().getCreatureStats(mFocusObject).getKnockedDown());
                }
                else if (loot)
                {
                    mModel = new InventoryItemModel(mFocusObject);
                }
                else 
                {
                    mModel = nullptr;
                    setVisibleAll(false);
                }
            }
            else 
            {
                mModel = new ContainerItemModel(mFocusObject);
            }

            if (mModel)
            {
                mSortModel = new SortFilterItemModel(mModel);
                mSortModel->setCategory(SortFilterItemModel::Category_Simple);
                mQuickLoot->setModel(mSortModel);
                mQuickLoot->update();
            }
            
            MWBase::Environment::get().getWindowManager()->setKeyFocusWidget(mQuickLoot);
           
            if (mQuickLoot->getModel())
                mLastIndex = mQuickLoot->forceItemFocused(0);

            if (mModel && mModel->getItemCount())
            {
                mLabel->setCaption(mFocusObject.getClass().getName(mFocusObject));
                setVisibleAll(true);
            }
            else 
            {
                setVisibleAll(false);
            }
        } 
    }

    void QuickLoot::position(MyGUI::IntPoint& position, MyGUI::IntSize size, MyGUI::IntSize viewportSize)
    {
        position += MyGUI::IntPoint(0, 32)
        - MyGUI::IntPoint(static_cast<int>(MyGUI::InputManager::getInstance().getMousePosition().left / float(viewportSize.width) * size.width), 0);

        if ((position.left + size.width) > viewportSize.width)
        {
            position.left = viewportSize.width - size.width;
        }
        if ((position.top + size.height) > viewportSize.height)
        {
            position.top = MyGUI::InputManager::getInstance().getMousePosition().top - size.height - 8;
        }
    }

    void QuickLoot::playOpenAnimation()
    {
        if (mFocusObject.isEmpty() || mFocusObject.getTypeName() != typeid(ESM::Container).name()) 
            return;
        auto* anim = MWBase::Environment::get().getWorld()->getAnimation(mFocusObject);

        if (!anim)
            return;

        if (!anim->hasAnimation("containeropen") || 
            anim->isPlaying("containeropen") || 
            anim->isPlaying("containerclose"))
            return;
        
        // super hacky :}
        mPlaying = true;
        anim->play("containeropen", MWMechanics::Priority_Persistent, MWRender::Animation::BlendMask_All, false, 1.0f, "start", "stop", 0.f, 0);
    }

    void QuickLoot::playCloseAnimation() const
    {
        if (mFocusObject.isEmpty() || mFocusObject.getTypeName() != typeid(ESM::Container).name()) 
            return;

        auto* anim = MWBase::Environment::get().getWorld()->getAnimation(mFocusObject);

        if (!anim)
            return;

        if (!anim->hasAnimation("containerclose"))
            return;

        float complete, startPoint = 0.f;
        bool animPlaying = anim->getInfo("containeropen", &complete);
        if (animPlaying)
            startPoint = 1.f - complete;

        anim->play("containerclose", MWMechanics::Priority_Persistent, MWRender::Animation::BlendMask_All, false, 1.0f, "start", "stop", startPoint, 0);
    }

    void QuickLoot::resize() 
    {
        const MyGUI::IntSize &viewSize = MyGUI::RenderManager::getInstance().getViewSize();

        MyGUI::IntSize tooltipSize = MyGUI::IntSize(mMainWidget->getWidth(),
            std::min(64 + mQuickLoot->requestListSize(),400));
        setCoord(viewSize.width*0.7 - tooltipSize.width/2,
                viewSize.height*0.6 - tooltipSize.height/2,
                tooltipSize.width,
                tooltipSize.height);
    }

    void QuickLoot::clear()
    {
        mFocusObject = MWWorld::Ptr();

        for (unsigned int i=0; i < mMainWidget->getChildCount(); ++i)
        {
            mMainWidget->getChildAt(i)->setVisible(false);
        }
    }

    void QuickLoot::setFocusObject(const MWWorld::Ptr& focus)
    {
        bool quickloot = Settings::Manager::getBool ("enable quickloot", "MorroUI");

        if (!mEnabled || !quickloot) return;

        auto player = MWMechanics::getPlayer();

        bool werewolf = player.getClass().getNpcStats(player).isWerewolf();
        bool incapacitated = (player.getClass().getCreatureStats(player).isParalyzed() || 
                             player.getClass().getCreatureStats(player).getKnockedDown());
        if (focus.isEmpty() || 
            MWBase::Environment::get().getWindowManager()->isGuiMode() || 
            werewolf ||
            incapacitated ||  
            (focus.getTypeName() != typeid(ESM::Container).name() && !focus.getClass().hasInventoryStore(focus)))
        {
            setVisibleAll(false);
            return;
        }

        if (focus != mFocusObject)
        {
            if (mOpened && !mShouldOpen)
                playCloseAnimation();

            mOpened = false;
            mShouldOpen = true;
        }
        
        mLastFocusObject = mFocusObject;
        mFocusObject = focus;

        bool combat = MWBase::Environment::get().getWorld()->getPlayer().isInCombat();
        bool loot = mFocusObject.getClass().isActor() && 
                    mFocusObject.getClass().getCreatureStats(mFocusObject).isDead() && 
                    mFocusObject.getClass().getCreatureStats(mFocusObject).isDeathAnimationFinished();
        bool sneaking = MWBase::Environment::get().getMechanicsManager()->isSneaking(player);
        bool hide = false;

        if (mFocusObject.getClass().hasInventoryStore(mFocusObject) && mFocusObject.getClass().isNpc())
                if (!loot && !sneaking || (!loot && sneaking && combat))
                    hide = true;

        if (mLastFocusObject == mFocusObject && !hide)
        {
            if (mQuickLoot->getModel())
            {
                mModel->update();
                mQuickLoot->update();
                if (mSortModel->getItemCount())
                {
                    mQuickLoot->forceItemFocused(mLastIndex);
                    mQuickLoot->update();

                    if (mLastIndex >= mSortModel->getItemCount())
                        mLastIndex = mSortModel->getItemCount()-1;

                    setVisibleAll(true);
                    if (MyGUI::InputManager::getInstance().getKeyFocusWidget() == nullptr)
                        MWBase::Environment::get().getWindowManager()->setKeyFocusWidget(mQuickLoot);
                }
                else if (loot && sneaking)
                {
                    update(mFrameDuration);
                    return;
                }
                else 
                    setVisibleAll(false);
                return;
            }
        }
        else 
        {
            setVisibleAll(false);
            mQuickLoot->setModel(nullptr);
            mSortModel = nullptr;
            mModel = nullptr;
        }
        update(mFrameDuration);
    }

    void QuickLoot::setFocusObjectScreenCoords(float min_x, float min_y, float max_x, float max_y)
    {
        mFocusToolTipX = (min_x + max_x) / 2;
        mFocusToolTipY = min_y;
    }

    void QuickLoot::setDelay(float delay)
    {
        mDelay = delay;
        mRemainingDelay = mDelay;
    }

}
