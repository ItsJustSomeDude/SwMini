#include "init/engine.h"
#include "components.h"

void *GlowComponent_Interface;
void *ManaComponent_Interface;
void *LightComponent_Interface;
void *ModelComponent_Interface;
void *ShapeComponent_Interface;
void *SkillComponent_Interface;
void *SpellComponent_Interface;
void *SwingComponent_Interface;
void *AttackComponent_Interface;
void *DamageComponent_Interface;
void *EntityComponent_Interface;
void *HealthComponent_Interface;
void *PortalComponent_Interface;
void *ShadowComponent_Interface;
void *SpriteComponent_Interface;
void *OverlayComponent_Interface;
void *ProgramComponent_Interface;
void *ShatterComponent_Interface;
void *ItemDropComponent_Interface;
void *ParticleComponent_Interface;
void *AnimationComponent_Interface;
void *MagicBoltComponent_Interface;
void *MagicBombComponent_Interface;
void *TouchableComponent_Interface;
void *TransformComponent_Interface;
void *WaterMeshComponent_Interface;
void *BackgroundComponent_Interface;
void *EntityInfoComponent_Interface;
void *FireBreathComponent_Interface;
void *GroundMeshComponent_Interface;
void *HeroEntityComponent_Interface;
void *PropertiesComponent_Interface;
void *SimpleGlowComponent_Interface;
void *SpawnPointComponent_Interface;
void *TextBubbleComponent_Interface;
void *WeaponGlowComponent_Interface;
void *FireEmitterComponent_Interface;
void *OverlayTextComponent_Interface;
void *SoundEffectComponent_Interface;
void *WeaponTrailComponent_Interface;
void *EntityActionComponent_Interface;
void *PortalEffectComponent_Interface;
void *ShadowVolumeComponent_Interface;
void *UtilityShapeComponent_Interface;
void *GroundPolygonComponent_Interface;
void *HookshotTrailComponent_Interface;
void *MagicHookshotComponent_Interface;
void *MonsterEntityComponent_Interface;
void *ParticleFieldComponent_Interface;
void *PhysicsObjectComponent_Interface;
void *BlendAnimationComponent_Interface;
void *BushControllerComponent_Interface;
void *CharControllerComponent_Interface;
void *CollisionShapeComponent_Interface;
void *DimensionSpellComponent_Interface;
void *DoorControllerComponent_Interface;
void *MagicExplosionComponent_Interface;
void *MagicSpellCastComponent_Interface;
void *ObjectModifierComponent_Interface;
void *ParticleObjectComponent_Interface;
void *TextureMappingComponent_Interface;
void *BreakableObjectComponent_Interface;
void *CollectableItemComponent_Interface;
void *DimensionObjectComponent_Interface;
void *OrbitControllerComponent_Interface;
void *ParticleEmitterComponent_Interface;
void *PhysicsPlatformComponent_Interface;
void *PressureTriggerComponent_Interface;
void *SwingableWeaponComponent_Interface;
void *EntityControllerComponent_Interface;
void *KeyframeAnimationComponent_Interface;
void *MonsterControllerComponent_Interface;
void *CharAnimControllerComponent_Interface;
void *ElevatorControllerComponent_Interface;
void *OverlayTargetArrowComponent_Interface;
void *RotatingBackgroundComponent_Interface;
void *AnimationControllerComponent_Interface;
void *GroundMeshGeneratorComponent_Interface;
void *TransformControllerComponent_Interface;
void *BatMonsterControllerComponent_Interface;
void *MagicParticleEmitterComponent_Interface;
void *ObjectLinkControllerComponent_Interface;
void *ProjectileControllerComponent_Interface;
void *MonsterDeathControllerComponent_Interface;
void *SkellyMonsterControllerComponent_Interface;
void *StaticMonsterControllerComponent_Interface;
void *GenericMonsterControllerComponent_Interface;
void *LeapingMonsterControllerComponent_Interface;
void *ModelTransformControllerComponent_Interface;
void *WalkingMonsterControllerComponent_Interface;
void *BouncingMonsterControllerComponent_Interface;
void *ChargingMonsterControllerComponent_Interface;
void *ShootingMonsterControllerComponent_Interface;
void *SnappingMonsterControllerComponent_Interface;
void *SwingableWeaponControllerComponent_Interface;
void *ProjectileMonsterControllerComponent_Interface;
void *BoneControlledCollisionShapeComponent_Interface;

DL_FUNCTION_SYMBOL(
	SceneObject_ComponentWithInterface,
	"_ZNK5Caver11SceneObject22ComponentWithInterfaceEl",
	void*, (const SceneObject *scene_object, void *interface)
)

void initE_components(void) {
	dlsym_SceneObject_ComponentWithInterface();

	GlowComponent_Interface = engine_dlsym("_ZN5Caver13GlowComponent9InterfaceEv");
	ManaComponent_Interface = engine_dlsym("_ZN5Caver13ManaComponent9InterfaceEv");
	LightComponent_Interface = engine_dlsym("_ZN5Caver14LightComponent9InterfaceEv");
	ModelComponent_Interface = engine_dlsym("_ZN5Caver14ModelComponent9InterfaceEv");
	ShapeComponent_Interface = engine_dlsym("_ZN5Caver14ShapeComponent9InterfaceEv");
	SkillComponent_Interface = engine_dlsym("_ZN5Caver14SkillComponent9InterfaceEv");
	SpellComponent_Interface = engine_dlsym("_ZN5Caver14SpellComponent9InterfaceEv");
	SwingComponent_Interface = engine_dlsym("_ZN5Caver14SwingComponent9InterfaceEv");
	AttackComponent_Interface = engine_dlsym("_ZN5Caver15AttackComponent9InterfaceEv");
	DamageComponent_Interface = engine_dlsym("_ZN5Caver15DamageComponent9InterfaceEv");
	EntityComponent_Interface = engine_dlsym("_ZN5Caver15EntityComponent9InterfaceEv");
	HealthComponent_Interface = engine_dlsym("_ZN5Caver15HealthComponent9InterfaceEv");
	PortalComponent_Interface = engine_dlsym("_ZN5Caver15PortalComponent9InterfaceEv");
	ShadowComponent_Interface = engine_dlsym("_ZN5Caver15ShadowComponent9InterfaceEv");
	SpriteComponent_Interface = engine_dlsym("_ZN5Caver15SpriteComponent9InterfaceEv");
	OverlayComponent_Interface = engine_dlsym("_ZN5Caver16OverlayComponent9InterfaceEv");
	ProgramComponent_Interface = engine_dlsym("_ZN5Caver16ProgramComponent9InterfaceEv");
	ShatterComponent_Interface = engine_dlsym("_ZN5Caver16ShatterComponent9InterfaceEv");
	ItemDropComponent_Interface = engine_dlsym("_ZN5Caver17ItemDropComponent9InterfaceEv");
	ParticleComponent_Interface = engine_dlsym("_ZN5Caver17ParticleComponent9InterfaceEv");
	AnimationComponent_Interface = engine_dlsym("_ZN5Caver18AnimationComponent9InterfaceEv");
	MagicBoltComponent_Interface = engine_dlsym("_ZN5Caver18MagicBoltComponent9InterfaceEv");
	MagicBombComponent_Interface = engine_dlsym("_ZN5Caver18MagicBombComponent9InterfaceEv");
	TouchableComponent_Interface = engine_dlsym("_ZN5Caver18TouchableComponent9InterfaceEv");
	TransformComponent_Interface = engine_dlsym("_ZN5Caver18TransformComponent9InterfaceEv");
	WaterMeshComponent_Interface = engine_dlsym("_ZN5Caver18WaterMeshComponent9InterfaceEv");
	BackgroundComponent_Interface = engine_dlsym("_ZN5Caver19BackgroundComponent9InterfaceEv");
	EntityInfoComponent_Interface = engine_dlsym("_ZN5Caver19EntityInfoComponent9InterfaceEv");
	FireBreathComponent_Interface = engine_dlsym("_ZN5Caver19FireBreathComponent9InterfaceEv");
	GroundMeshComponent_Interface = engine_dlsym("_ZN5Caver19GroundMeshComponent9InterfaceEv");
	HeroEntityComponent_Interface = engine_dlsym("_ZN5Caver19HeroEntityComponent9InterfaceEv");
	PropertiesComponent_Interface = engine_dlsym("_ZN5Caver19PropertiesComponent9InterfaceEv");
	SimpleGlowComponent_Interface = engine_dlsym("_ZN5Caver19SimpleGlowComponent9InterfaceEv");
	SpawnPointComponent_Interface = engine_dlsym("_ZN5Caver19SpawnPointComponent9InterfaceEv");
	TextBubbleComponent_Interface = engine_dlsym("_ZN5Caver19TextBubbleComponent9InterfaceEv");
	WeaponGlowComponent_Interface = engine_dlsym("_ZN5Caver19WeaponGlowComponent9InterfaceEv");
	FireEmitterComponent_Interface = engine_dlsym("_ZN5Caver20FireEmitterComponent9InterfaceEv");
	OverlayTextComponent_Interface = engine_dlsym("_ZN5Caver20OverlayTextComponent9InterfaceEv");
	SoundEffectComponent_Interface = engine_dlsym("_ZN5Caver20SoundEffectComponent9InterfaceEv");
	WeaponTrailComponent_Interface = engine_dlsym("_ZN5Caver20WeaponTrailComponent9InterfaceEv");
	EntityActionComponent_Interface = engine_dlsym("_ZN5Caver21EntityActionComponent9InterfaceEv");
	PortalEffectComponent_Interface = engine_dlsym("_ZN5Caver21PortalEffectComponent9InterfaceEv");
	ShadowVolumeComponent_Interface = engine_dlsym("_ZN5Caver21ShadowVolumeComponent9InterfaceEv");
	UtilityShapeComponent_Interface = engine_dlsym("_ZN5Caver21UtilityShapeComponent9InterfaceEv");
	GroundPolygonComponent_Interface = engine_dlsym(
		"_ZN5Caver22GroundPolygonComponent9InterfaceEv");
	HookshotTrailComponent_Interface = engine_dlsym(
		"_ZN5Caver22HookshotTrailComponent9InterfaceEv");
	MagicHookshotComponent_Interface = engine_dlsym(
		"_ZN5Caver22MagicHookshotComponent9InterfaceEv");
	MonsterEntityComponent_Interface = engine_dlsym(
		"_ZN5Caver22MonsterEntityComponent9InterfaceEv");
	ParticleFieldComponent_Interface = engine_dlsym(
		"_ZN5Caver22ParticleFieldComponent9InterfaceEv");
	PhysicsObjectComponent_Interface = engine_dlsym(
		"_ZN5Caver22PhysicsObjectComponent9InterfaceEv");
	BlendAnimationComponent_Interface = engine_dlsym(
		"_ZN5Caver23BlendAnimationComponent9InterfaceEv");
	BushControllerComponent_Interface = engine_dlsym(
		"_ZN5Caver23BushControllerComponent9InterfaceEv");
	CharControllerComponent_Interface = engine_dlsym(
		"_ZN5Caver23CharControllerComponent9InterfaceEv");
	CollisionShapeComponent_Interface = engine_dlsym(
		"_ZN5Caver23CollisionShapeComponent9InterfaceEv");
	DimensionSpellComponent_Interface = engine_dlsym(
		"_ZN5Caver23DimensionSpellComponent9InterfaceEv");
	DoorControllerComponent_Interface = engine_dlsym(
		"_ZN5Caver23DoorControllerComponent9InterfaceEv");
	MagicExplosionComponent_Interface = engine_dlsym(
		"_ZN5Caver23MagicExplosionComponent9InterfaceEv");
	MagicSpellCastComponent_Interface = engine_dlsym(
		"_ZN5Caver23MagicSpellCastComponent9InterfaceEv");
	ObjectModifierComponent_Interface = engine_dlsym(
		"_ZN5Caver23ObjectModifierComponent9InterfaceEv");
	ParticleObjectComponent_Interface = engine_dlsym(
		"_ZN5Caver23ParticleObjectComponent9InterfaceEv");
	TextureMappingComponent_Interface = engine_dlsym(
		"_ZN5Caver23TextureMappingComponent9InterfaceEv");
	BreakableObjectComponent_Interface = engine_dlsym(
		"_ZN5Caver24BreakableObjectComponent9InterfaceEv");
	CollectableItemComponent_Interface = engine_dlsym(
		"_ZN5Caver24CollectableItemComponent9InterfaceEv");
	DimensionObjectComponent_Interface = engine_dlsym(
		"_ZN5Caver24DimensionObjectComponent9InterfaceEv");
	OrbitControllerComponent_Interface = engine_dlsym(
		"_ZN5Caver24OrbitControllerComponent9InterfaceEv");
	ParticleEmitterComponent_Interface = engine_dlsym(
		"_ZN5Caver24ParticleEmitterComponent9InterfaceEv");
	PhysicsPlatformComponent_Interface = engine_dlsym(
		"_ZN5Caver24PhysicsPlatformComponent9InterfaceEv");
	PressureTriggerComponent_Interface = engine_dlsym(
		"_ZN5Caver24PressureTriggerComponent9InterfaceEv");
	SwingableWeaponComponent_Interface = engine_dlsym(
		"_ZN5Caver24SwingableWeaponComponent9InterfaceEv");
	EntityControllerComponent_Interface = engine_dlsym(
		"_ZN5Caver25EntityControllerComponent9InterfaceEv");
	KeyframeAnimationComponent_Interface = engine_dlsym(
		"_ZN5Caver26KeyframeAnimationComponent9InterfaceEv");
	MonsterControllerComponent_Interface = engine_dlsym(
		"_ZN5Caver26MonsterControllerComponent9InterfaceEv");
	CharAnimControllerComponent_Interface = engine_dlsym(
		"_ZN5Caver27CharAnimControllerComponent9InterfaceEv");
	ElevatorControllerComponent_Interface = engine_dlsym(
		"_ZN5Caver27ElevatorControllerComponent9InterfaceEv");
	OverlayTargetArrowComponent_Interface = engine_dlsym(
		"_ZN5Caver27OverlayTargetArrowComponent9InterfaceEv");
	RotatingBackgroundComponent_Interface = engine_dlsym(
		"_ZN5Caver27RotatingBackgroundComponent9InterfaceEv");
	AnimationControllerComponent_Interface = engine_dlsym(
		"_ZN5Caver28AnimationControllerComponent9InterfaceEv");
	GroundMeshGeneratorComponent_Interface = engine_dlsym(
		"_ZN5Caver28GroundMeshGeneratorComponent9InterfaceEv");
	TransformControllerComponent_Interface = engine_dlsym(
		"_ZN5Caver28TransformControllerComponent9InterfaceEv");
	BatMonsterControllerComponent_Interface = engine_dlsym(
		"_ZN5Caver29BatMonsterControllerComponent9InterfaceEv");
	MagicParticleEmitterComponent_Interface = engine_dlsym(
		"_ZN5Caver29MagicParticleEmitterComponent9InterfaceEv");
	ObjectLinkControllerComponent_Interface = engine_dlsym(
		"_ZN5Caver29ObjectLinkControllerComponent9InterfaceEv");
	ProjectileControllerComponent_Interface = engine_dlsym(
		"_ZN5Caver29ProjectileControllerComponent9InterfaceEv");
	MonsterDeathControllerComponent_Interface = engine_dlsym(
		"_ZN5Caver31MonsterDeathControllerComponent9InterfaceEv");
	SkellyMonsterControllerComponent_Interface = engine_dlsym(
		"_ZN5Caver32SkellyMonsterControllerComponent9InterfaceEv");
	StaticMonsterControllerComponent_Interface = engine_dlsym(
		"_ZN5Caver32StaticMonsterControllerComponent9InterfaceEv");
	GenericMonsterControllerComponent_Interface = engine_dlsym(
		"_ZN5Caver33GenericMonsterControllerComponent9InterfaceEv");
	LeapingMonsterControllerComponent_Interface = engine_dlsym(
		"_ZN5Caver33LeapingMonsterControllerComponent9InterfaceEv");
	ModelTransformControllerComponent_Interface = engine_dlsym(
		"_ZN5Caver33ModelTransformControllerComponent9InterfaceEv");
	WalkingMonsterControllerComponent_Interface = engine_dlsym(
		"_ZN5Caver33WalkingMonsterControllerComponent9InterfaceEv");
	BouncingMonsterControllerComponent_Interface = engine_dlsym(
		"_ZN5Caver34BouncingMonsterControllerComponent9InterfaceEv");
	ChargingMonsterControllerComponent_Interface = engine_dlsym(
		"_ZN5Caver34ChargingMonsterControllerComponent9InterfaceEv");
	ShootingMonsterControllerComponent_Interface = engine_dlsym(
		"_ZN5Caver34ShootingMonsterControllerComponent9InterfaceEv");
	SnappingMonsterControllerComponent_Interface = engine_dlsym(
		"_ZN5Caver34SnappingMonsterControllerComponent9InterfaceEv");
	SwingableWeaponControllerComponent_Interface = engine_dlsym(
		"_ZN5Caver34SwingableWeaponControllerComponent9InterfaceEv");
	ProjectileMonsterControllerComponent_Interface = engine_dlsym(
		"_ZN5Caver36ProjectileMonsterControllerComponent9InterfaceEv");
	BoneControlledCollisionShapeComponent_Interface = engine_dlsym(
		"_ZN5Caver37BoneControlledCollisionShapeComponent9InterfaceEv");
}
