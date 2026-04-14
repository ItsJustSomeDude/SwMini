#ifndef NEEDLE_CAVER_COMPONENTS_H
#define NEEDLE_CAVER_COMPONENTS_H

#include "core/hooks.h"
#include "scene_object.h"

/* Bash command to dump the interface list:
 * Headers:
	nm -gDC -f just-symbols libengine.so | grep -Po '(?<=Caver::).+?Component(?=::Interface\(\))' | xargs -I{} echo "extern void *{}_Interface;"
 * Impl:
	nm -gDC -f just-symbols libengine.so | grep -Po '(?<=Caver::).+?Component(?=::Interface\(\))' | xargs -I{} echo "extern void *{}_Interface;"
 * Setters:
	nm -gD -f just-symbols libengine.so | grep -P '5Caver\d+.+?Component(?=9InterfaceEv)' |
		while read; do sym="$(c++filt "${REPLY}" | grep -Po 'Caver::\K.+?(?=::)')"; echo "${sym}_Interface = engine_dlsym(\"${REPLY}\");"; done
 */

typedef struct Component Component;

extern void *GlowComponent_Interface;
extern void *ManaComponent_Interface;
extern void *LightComponent_Interface;
extern void *ModelComponent_Interface;
extern void *ShapeComponent_Interface;
extern void *SkillComponent_Interface;
extern void *SpellComponent_Interface;
extern void *SwingComponent_Interface;
extern void *AttackComponent_Interface;
extern void *DamageComponent_Interface;
extern void *EntityComponent_Interface;
extern void *HealthComponent_Interface;
extern void *PortalComponent_Interface;
extern void *ShadowComponent_Interface;
extern void *SpriteComponent_Interface;
extern void *OverlayComponent_Interface;
extern void *ProgramComponent_Interface;
extern void *ShatterComponent_Interface;
extern void *ItemDropComponent_Interface;
extern void *ParticleComponent_Interface;
extern void *AnimationComponent_Interface;
extern void *MagicBoltComponent_Interface;
extern void *MagicBombComponent_Interface;
extern void *TouchableComponent_Interface;
extern void *TransformComponent_Interface;
extern void *WaterMeshComponent_Interface;
extern void *BackgroundComponent_Interface;
extern void *EntityInfoComponent_Interface;
extern void *FireBreathComponent_Interface;
extern void *GroundMeshComponent_Interface;
extern void *HeroEntityComponent_Interface;
extern void *PropertiesComponent_Interface;
extern void *SimpleGlowComponent_Interface;
extern void *SpawnPointComponent_Interface;
extern void *TextBubbleComponent_Interface;
extern void *WeaponGlowComponent_Interface;
extern void *FireEmitterComponent_Interface;
extern void *OverlayTextComponent_Interface;
extern void *SoundEffectComponent_Interface;
extern void *WeaponTrailComponent_Interface;
extern void *EntityActionComponent_Interface;
extern void *PortalEffectComponent_Interface;
extern void *ShadowVolumeComponent_Interface;
extern void *UtilityShapeComponent_Interface;
extern void *GroundPolygonComponent_Interface;
extern void *HookshotTrailComponent_Interface;
extern void *MagicHookshotComponent_Interface;
extern void *MonsterEntityComponent_Interface;
extern void *ParticleFieldComponent_Interface;
extern void *PhysicsObjectComponent_Interface;
extern void *BlendAnimationComponent_Interface;
extern void *BushControllerComponent_Interface;
extern void *CharControllerComponent_Interface;
extern void *CollisionShapeComponent_Interface;
extern void *DimensionSpellComponent_Interface;
extern void *DoorControllerComponent_Interface;
extern void *MagicExplosionComponent_Interface;
extern void *MagicSpellCastComponent_Interface;
extern void *ObjectModifierComponent_Interface;
extern void *ParticleObjectComponent_Interface;
extern void *TextureMappingComponent_Interface;
extern void *BreakableObjectComponent_Interface;
extern void *CollectableItemComponent_Interface;
extern void *DimensionObjectComponent_Interface;
extern void *OrbitControllerComponent_Interface;
extern void *ParticleEmitterComponent_Interface;
extern void *PhysicsPlatformComponent_Interface;
extern void *PressureTriggerComponent_Interface;
extern void *SwingableWeaponComponent_Interface;
extern void *EntityControllerComponent_Interface;
extern void *KeyframeAnimationComponent_Interface;
extern void *MonsterControllerComponent_Interface;
extern void *CharAnimControllerComponent_Interface;
extern void *ElevatorControllerComponent_Interface;
extern void *OverlayTargetArrowComponent_Interface;
extern void *RotatingBackgroundComponent_Interface;
extern void *AnimationControllerComponent_Interface;
extern void *GroundMeshGeneratorComponent_Interface;
extern void *TransformControllerComponent_Interface;
extern void *BatMonsterControllerComponent_Interface;
extern void *MagicParticleEmitterComponent_Interface;
extern void *ObjectLinkControllerComponent_Interface;
extern void *ProjectileControllerComponent_Interface;
extern void *MonsterDeathControllerComponent_Interface;
extern void *SkellyMonsterControllerComponent_Interface;
extern void *StaticMonsterControllerComponent_Interface;
extern void *GenericMonsterControllerComponent_Interface;
extern void *LeapingMonsterControllerComponent_Interface;
extern void *ModelTransformControllerComponent_Interface;
extern void *WalkingMonsterControllerComponent_Interface;
extern void *BouncingMonsterControllerComponent_Interface;
extern void *ChargingMonsterControllerComponent_Interface;
extern void *ShootingMonsterControllerComponent_Interface;
extern void *SnappingMonsterControllerComponent_Interface;
extern void *SwingableWeaponControllerComponent_Interface;
extern void *ProjectileMonsterControllerComponent_Interface;
extern void *BoneControlledCollisionShapeComponent_Interface;

H_DL_FUNCTION(
	SceneObject_ComponentWithInterface,
	void*, (const SceneObject *scene_object, const void *interface)
)

#endif /* NEEDLE_CAVER_COMPONENTS_H */
