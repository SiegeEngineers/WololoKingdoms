#include "balance.h"
#include "wololo/datPatch.h"

namespace wololo {

// Applies balances changes by mutating data file `aocDat`.
void balancePatch(genie::DatFile* aocDat) {

  const int tech_id_new = 801;
  const int effect_id_new = 798;

  // Creates the Supplies technology and effect.
  const int tech_id_supplies = aocDat->Techs.size();
  const int effect_id_supplies = aocDat->Effects.size();

  auto effect_supplies = aocDat->Effects[effect_id_new];
  effect_supplies.Name = "Supplies";
  auto& ec_supplies = effect_supplies.EffectCommands;
  const int swordsmen[] = {74, 75, 77, 473, 567};
  for (int unit_id : swordsmen) {
    auto ec = genie::EffectCommand();
    ec.Type = genie::EffectCommand::EffectType::RelativeAttributeModifier;
    ec.TargetUnit = unit_id;
    ec.AttributeID = genie::EffectCommand::Attributes::FoodCosts;
    ec.Amount = -15;
    ec_supplies.push_back(ec);
  }

  auto tech_supplies = aocDat->Techs[tech_id_new];
  tech_supplies.Name = "Supplies";
  tech_supplies.Name2 = "Supplies";
  tech_supplies.RequiredTechs[0] = 101; // Feudal Age
  tech_supplies.RequiredTechCount = 1;
  tech_supplies.ResearchTime = 35;
  // TODO needs an icon
  tech_supplies.ButtonID = 11;
  tech_supplies.EffectID = effect_id_supplies;
  tech_supplies.ResearchLocation = 12; // Barracks
  tech_supplies.ResourceCosts[0].Type = 0;
  tech_supplies.ResourceCosts[0].Amount = 150; // Food Storage
  tech_supplies.ResourceCosts[0].Paid = 1;
  tech_supplies.ResourceCosts[1].Type = 3;
  tech_supplies.ResourceCosts[1].Amount = 100; // Gold Storage
  tech_supplies.ResourceCosts[1].Paid = 1;

  aocDat->Effects.push_back(effect_supplies);
  aocDat->Techs.push_back(tech_supplies);

  // Makes Tracking free and instant upon reaching the Feudal Age.
  const int tech_id_tracking = 90;
  aocDat->Techs[tech_id_tracking].ResearchLocation = -1;
  aocDat->Techs[tech_id_tracking].ResearchTime = 0;
  // Copies an empty entry to overwrite the Food entry.
  aocDat->Techs[tech_id_tracking].ResourceCosts[0] =
      aocDat->Techs[tech_id_tracking].ResourceCosts[1];

  // Sets Elite Skirmisher cost to 230w/130g.
  const int tech_id_elite_skirmisher = 98;
  aocDat->Techs[tech_id_tracking].ResearchLocation = -1;
  aocDat->Techs[tech_id_elite_skirmisher].ResourceCosts[1].Amount = 130;

  // Sets Herbal Medicine to Multiply Garrison Heal Rate by 6.
  const int effect_id_herbal_medicine = 41;
  aocDat->Effects[effect_id_herbal_medicine].EffectCommands[0].Amount = 6;
  aocDat->Effects[effect_id_herbal_medicine].EffectCommands[1].Amount = 6;

  // Sets Archer Attack Animation Duration to 0.7.
  const int graphic_id_archer_attack = 627;
  // 0.7 just sets the value to 7, used 0.07 to get 0.7 in the box in AGE.
  aocDat->Graphics[graphic_id_archer_attack].FrameDuration = 0.07f;

  // Sets Palisade Gate and Gate foundation Class 3 and 4 armor to 0.
  const int unit_id_gate_foundations[] = {487, 488, 490, 491, 665, 666,
                                          673, 674, 792, 796, 800, 804};
  for (auto& civ : aocDat->Civs) {
    for (int unitId : unit_id_gate_foundations) {
      auto& gate_foundation_armors = civ.Units[unitId].Combat.Armours;
      gate_foundation_armors[2].Amount = 0;
      gate_foundation_armors[3].Amount = 0;
    }
  }

  // Sets Palisade Gate cost to 30 wood.
  const int unit_id_palisade_gate_start = 789;
  const int unit_id_palisade_gate_end = 804;
  for (auto& civ : aocDat->Civs) {
    for (int unitId = unit_id_palisade_gate_start;
         unitId <= unit_id_palisade_gate_end; ++unitId) {
      civ.Units[unitId].Creatable.ResourceCosts[0].Amount = 30;
    }
  }

  // Makes Watch Towers have 700 HP in Dark and Feudal Ages
  const int unit_id_watch_tower = 79;

  // Sets the Watch Tower HP to 700.
  for (auto& civ : aocDat->Civs) {
    civ.Units[unit_id_watch_tower].HitPoints = 700;
  }

  // Creates a free HP upgrade to 1020 when reaching the Castle Age.
  auto effect_id_watch_tower_hp = aocDat->Effects.size();

  auto effect_watch_tower_hp = aocDat->Effects[effect_id_new];
  effect_watch_tower_hp.Name = "Watch Tower HP";
  auto effect_watch_tower_hp_command = genie::EffectCommand();
  effect_watch_tower_hp_command.Type =
      genie::EffectCommand::EffectType::AbsoluteAttributeModifier;
  effect_watch_tower_hp_command.TargetUnit = unit_id_watch_tower;
  effect_watch_tower_hp_command.UnitClassID = -1;
  effect_watch_tower_hp_command.AttributeID =
      genie::EffectCommand::Attributes::HitPoints;
  effect_watch_tower_hp_command.Amount = 1020;
  effect_watch_tower_hp.EffectCommands.push_back(effect_watch_tower_hp_command);
  aocDat->Effects.push_back(effect_watch_tower_hp);

  auto tech_watch_tower_hp = aocDat->Techs[tech_id_new];
  tech_watch_tower_hp.Name = "Watch Tower HP";
  tech_watch_tower_hp.Name2 = "Watch Tower HP";
  tech_watch_tower_hp.RequiredTechs[0] = 102;
  tech_watch_tower_hp.RequiredTechCount = 1;
  tech_watch_tower_hp.EffectID = effect_id_watch_tower_hp;
  aocDat->Techs.push_back(tech_watch_tower_hp);

  // Sets Elite Battle Elephant melee attack to 14.
  const int unit_id_elite_battle_elephant = 766;
  for (auto& civ : aocDat->Civs) {
    civ.Units[unit_id_elite_battle_elephant].Combat.Attacks[1].Amount = 14;
  }

  // Makes Dry Dock maintain the Trade Cog gold per trip amount.
  const int effect_id_dry_dock = 373;
  auto ec_dry_dock_speed = genie::EffectCommand();
  ec_dry_dock_speed.Type =
      genie::EffectCommand::EffectType::AttributeMultiplier;
  ec_dry_dock_speed.TargetUnit = -1;
  ec_dry_dock_speed.UnitClassID = 2; // Trade Boat class
  ec_dry_dock_speed.AttributeID = genie::EffectCommand::Attributes::WorkRate;
  ec_dry_dock_speed.Amount = 1.15f;
  aocDat->Effects[effect_id_dry_dock].EffectCommands.push_back(ec_dry_dock_speed);

  // Makes the Berber ship speed bonus maintain the Trade Cog gold per trip
  // amount.
  const int effect_id_berber_ship_speed = 611;
  auto ec_berber_ship_speed = genie::EffectCommand();
  ec_berber_ship_speed.Type =
      genie::EffectCommand::EffectType::AttributeMultiplier;
  ec_berber_ship_speed.TargetUnit = -1;
  ec_berber_ship_speed.UnitClassID = 2; // Trade Boat class
  ec_berber_ship_speed.AttributeID = genie::EffectCommand::Attributes::WorkRate;
  ec_berber_ship_speed.Amount = 1.1f;
  aocDat->Effects[effect_id_berber_ship_speed].EffectCommands.push_back(
      ec_berber_ship_speed);

  // Gives Eagle Scouts 5 LoS and Search radius in the Dark Age, 6 in the Feudal
  // Age.

  // Sets the base values to 5.
  const int unit_id_eagle_scout = 751;
  for (auto& civ : aocDat->Civs) {
    civ.Units[unit_id_eagle_scout].LineOfSight = 5;
    civ.Units[unit_id_eagle_scout].Action.SearchRadius = 5;
  }

  // Creates a free upgrade to 6, researched upon reaching the Feudal Age.
  auto effect_id_eagle_scout_los = aocDat->Effects.size();

  auto effect_eagle_scout_los = aocDat->Effects[effect_id_new];
  effect_eagle_scout_los.Name = "Eagle Scout Feudal LoS";

  auto effect_eagle_scout_los_command = genie::EffectCommand();
  effect_eagle_scout_los_command.Type =
      genie::EffectCommand::EffectType::AbsoluteAttributeModifier;
  effect_eagle_scout_los_command.TargetUnit = unit_id_eagle_scout;
  effect_eagle_scout_los_command.UnitClassID = -1;
  effect_eagle_scout_los_command.AttributeID =
      genie::EffectCommand::Attributes::LineOfSight;
  effect_eagle_scout_los_command.Amount = 6;
  effect_eagle_scout_los.EffectCommands.push_back(
      effect_eagle_scout_los_command);

  auto effect_eagle_scout_sr_command = genie::EffectCommand();
  effect_eagle_scout_sr_command.Type =
      genie::EffectCommand::EffectType::AbsoluteAttributeModifier;
  effect_eagle_scout_sr_command.TargetUnit = unit_id_eagle_scout;
  effect_eagle_scout_sr_command.UnitClassID = -1;
  effect_eagle_scout_sr_command.AttributeID =
      genie::EffectCommand::Attributes::SearchRadius;
  effect_eagle_scout_sr_command.Amount = 6;
  effect_eagle_scout_los.EffectCommands.push_back(
      effect_eagle_scout_sr_command);

  aocDat->Effects.push_back(effect_eagle_scout_los);

  auto tech_eagle_scout_los = aocDat->Techs[tech_id_new];
  tech_eagle_scout_los.Name = "Eagle Scout Feudal LoS";
  tech_eagle_scout_los.Name2 = "Eagle Scout Feudal LoS";
  tech_eagle_scout_los.RequiredTechs[0] = 101;
  tech_eagle_scout_los.RequiredTechCount = 1;
  tech_eagle_scout_los.EffectID = effect_id_eagle_scout_los;
  aocDat->Techs.push_back(tech_eagle_scout_los);

  // Makes trebs do damage if the shot lands after they pack.
  const int unit_id_treb = 42;
  const int unit_id_treb_packed = 331;
  for (auto& civ : aocDat->Civs) {
    civ.Units[unit_id_treb_packed].Combat.Attacks =
        civ.Units[unit_id_treb].Combat.Attacks;
  }

  // Sets Genitour and Elite Genitour attack vs Spearman line to 2.
  const int unit_id_genitour = 223;
  const int unit_id_elite_genitour = 230;
  for (auto& civ : aocDat->Civs) {
    civ.Units[unit_id_genitour].Combat.Attacks[5].Amount = 2;
    civ.Units[unit_id_elite_genitour].Combat.Attacks[5].Amount = 2;
  }

  // Sets Warwolf cost to 800w/400g.
  const int tech_id_warwolf = 461;
  aocDat->Techs[tech_id_warwolf].ResourceCosts[0].Amount = 800;
  aocDat->Techs[tech_id_warwolf].ResourceCosts[1].Amount = 400;

  // Starts the Franks Cavalry HP bonus in the Feudal Age
  const int tech_id_frank_cav_hp = 290;
  aocDat->Techs[tech_id_frank_cav_hp].Name = "Franks 20% Cavalry HP";
  aocDat->Techs[tech_id_frank_cav_hp].Name2 = "Franks 20% Cavalry HP";
  aocDat->Techs[tech_id_frank_cav_hp].RequiredTechs[0] = 101;
  aocDat->Techs[tech_id_frank_cav_hp].RequiredTechCount = 1;

  // Sets Slinger melee attack to 4.
  const int unit_id_slinger = 185;
  for (auto& civ : aocDat->Civs) {
    civ.Units[unit_id_slinger].Combat.Attacks[2].Amount = 4;
  }

  // Sets Genoese Xbow Reload Time to 2.
  const int unit_id_genoese_xbow = 866;
  for (auto& civ : aocDat->Civs) {
    civ.Units[unit_id_genoese_xbow].Combat.ReloadTime = 2;
  }

  // Makes Chemistry give +1 attack to secondary Castle arrows.
  const int effect_id_chemistry = 47;
  auto& effect_commands_chemistry =
      aocDat->Effects[effect_id_chemistry].EffectCommands;
  auto ec_chem_44 = effect_commands_chemistry[44];
  effect_commands_chemistry[44] = effect_commands_chemistry[45];
  effect_commands_chemistry[45] = ec_chem_44;

  // Makes Chemistry give +1 attack to secondary Ballista Elephant projectiles.
  // Ok, let's print an example
  auto example = effect_commands_chemistry[38]; // arrow
  const int pierce_attack_type = 3;
  const int unit_id_ballista_ele_secondary_bolts[] = {1167, 1168};
  for (int unit_id : unit_id_ballista_ele_secondary_bolts) {
    auto ec = genie::EffectCommand();
    ec.Type = genie::EffectCommand::EffectCommand::RelativeAttributeModifier;
    ec.TargetUnit = unit_id;
    ec.UnitClassID = -1;
    ec.AttributeID = genie::EffectCommand::Attributes::Attack;
    ec.Amount = pierce_attack_type * 256 + 1;
    effect_commands_chemistry.push_back(ec);
  }

  // Makes Khmer receive faith and hussar while not receiving shipwright.
  const int effect_id_khmer_tech_tree = 646;
  const int tech_id_shipwright = 373;
  auto& effect_commands_khmer =
      aocDat->Effects[effect_id_khmer_tech_tree].EffectCommands;
  // Switches Effect Command 16 from disabling Hussar to disabling Shipwright
  // (373).
  effect_commands_khmer[5].Amount = tech_id_shipwright;
  // Deletes the effect command that disables Faith.
  effect_commands_khmer.erase(effect_commands_khmer.begin() + 16);

  // Changes the Korean Fortification Speed bonus to the Wood Cost reduction
  // bouns.
  const int tech_id_koreans_bonus = 390;
  aocDat->Techs[tech_id_koreans_bonus].Name = "C 18, C-Bonus, Units -15% wood cost";
  aocDat->Techs[tech_id_koreans_bonus].Name2 = "C 18, C-Bonus, Units -15% wood cost";
  const int effect_id_koreans_bonus = 561;
  aocDat->Effects[effect_id_koreans_bonus].Name = "C-Bonus, Units -15% wood cost";
  const int wood_bonus_classes[] = {0, 6, 12, 22, 36};
  auto& effect_commands_koreans_bonus =
      aocDat->Effects[effect_id_koreans_bonus].EffectCommands;
  for (int i = 0; i != effect_commands_koreans_bonus.size(); ++i) {
    auto& ec = effect_commands_koreans_bonus[i];
    ec.UnitClassID = wood_bonus_classes[i];
    ec.AttributeID = genie::EffectCommand::Attributes::WoodCosts;
    ec.Amount = 0.85f;
  }

  // Sets Malay harbors to have 3 pierce attack, 10 ship and fishing ship
  // attack, and 1 minimum range.
  const int unit_id_harbor = 1189;
  for (auto& civ : aocDat->Civs) {
    auto& harbor_unit_combat = civ.Units[unit_id_harbor].Combat;
    harbor_unit_combat.MinRange = 1;
    auto& attacks_harbor = harbor_unit_combat.Attacks;
    attacks_harbor[0].Amount = 10;
    attacks_harbor[1].Amount = 3;
    attacks_harbor[2].Amount = 10;
  }

  // Makes Forced Levy increase food costs.
  const int effect_id_forced_levy = 665;
  const int num_cmds = aocDat->Effects[effect_id_forced_levy].EffectCommands.size();
  for (int i = 0; i != num_cmds; ++i) {
    auto ec = aocDat->Effects[effect_id_forced_levy].EffectCommands[i];
    ec.AttributeID = genie::EffectCommand::Attributes::FoodCosts;
    ec.Amount = 20;
    aocDat->Effects[effect_id_forced_levy].EffectCommands.push_back(ec);
  }

  // Sets Forced Levy cost to 850f/500g.
  const int tech_id_forced_levy = 625;
  aocDat->Techs[tech_id_forced_levy].ResourceCosts[0].Amount = 850;
  aocDat->Techs[tech_id_forced_levy].ResourceCosts[1].Amount = 500;

  // Makes Malians receive Fast Fire Ship instead of Galleon
  const int effect_id_malian_tech_tree = 42;
  const int tech_id_galleon = 35;
  aocDat->Effects[effect_id_malian_tech_tree].EffectCommands[0].Amount =
      tech_id_galleon;

  // Sets Plumed Archer and Elite Plumed Archer cost to 55w/55g.
  const int unit_id_plumed_archer = 763;
  const int unit_id_elite_plumed_archer = 765;
  for (auto& civ : aocDat->Civs) {
    auto& plumed_archer_res_costs =
        civ.Units[unit_id_plumed_archer].Creatable.ResourceCosts;
    plumed_archer_res_costs[0].Amount = 55;
    plumed_archer_res_costs[1].Amount = 55;
    auto& elite_plumed_archer_res_costs =
        civ.Units[unit_id_elite_plumed_archer].Creatable.ResourceCosts;
    elite_plumed_archer_res_costs[0].Amount = 55;
    elite_plumed_archer_res_costs[1].Amount = 55;
  }

  // Gives Persian TCs and Docks +5% production bonus in the Dark Age.
  const int effect_id_persian_tc = 421;
  const int effect_id_persian_dock = 424;
  aocDat->Effects[effect_id_persian_tc].EffectCommands[0].Amount = 1.05f;
  aocDat->Effects[effect_id_persian_dock].EffectCommands[0].Amount = 1.05f;

  // Removes Boiling Oil from and adds Kamandaran to the Persians.
  const int tech_id_persians_ut = 488;
  auto& tech_kamandaran = aocDat->Techs[tech_id_persians_ut];
  tech_kamandaran.Name = "Kamandaran";
  tech_kamandaran.Name2 = "Kamandaran";
  tech_kamandaran.ResourceCosts[0].Type = 0; // Food Storage
  tech_kamandaran.ResourceCosts[1].Type = 3; // Gold Storage
  tech_kamandaran.ResourceCosts[1].Amount = 200;

  const int effect_id_persians_ut = 543;
  aocDat->Effects[effect_id_persians_ut].Name = "Kamandaran";
  auto& ec_persians_ut = aocDat->Effects[effect_id_persians_ut].EffectCommands;
  ec_persians_ut.clear();

  const int unit_id_archers[] = {4, 24, 492};
  for (int unit_id : unit_id_archers) {
    auto ec_gold = genie::EffectCommand();
    ec_gold.Type = genie::EffectCommand::EffectType::RelativeAttributeModifier;
    ec_gold.TargetUnit = unit_id;
    ec_gold.AttributeID = genie::EffectCommand::Attributes::GoldCosts;
    ec_gold.Amount = -45;
    ec_persians_ut.push_back(ec_gold);

    auto ec_wood = ec_gold;
    ec_wood.AttributeID = genie::EffectCommand::Attributes::WoodCosts;
    ec_wood.Amount = 25;
    ec_persians_ut.push_back(ec_wood);
  }

  // Gives Saracen Archers +1/+1/+1 and CA +2/+1/+1 attack bonus vs buildings.
  const int class_cav_archer = 36;
  const int tech_id_saracen_archer2 = 312;
  const int effect_id_saracen_archer2 = 312;
  const int building_attack_base = 21 * 256;
  const int civ_id_saracens = 9;
  aocDat->Techs[tech_id_saracen_archer2].Name =
      "C 9, C-Bonus, Archers v Buildings 2";
  aocDat->Techs[tech_id_saracen_archer2].Name2 =
      "C 9, C-Bonus, Archers v Buildings 2";
  aocDat->Effects[effect_id_saracen_archer2].Name =
      "C 9, C-Bonus, Archers v Buildings 2";

  // Sets the Feudal Age Effect Commands.
  auto& ec_sara_2 = aocDat->Effects[effect_id_saracen_archer2].EffectCommands;
  ec_sara_2.clear();

  auto ec_sara_2_ca = genie::EffectCommand();
  ec_sara_2_ca.Type =
      genie::EffectCommand::EffectType::RelativeAttributeModifier;
  ec_sara_2_ca.TargetUnit = -1;
  ec_sara_2_ca.UnitClassID = 36;
  ec_sara_2_ca.AttributeID = genie::EffectCommand::Attributes::Attack;
  ec_sara_2_ca.Amount = building_attack_base + 2;
  ec_sara_2.push_back(ec_sara_2_ca);

  for (int unit_id : unit_id_archers) {
    auto ec_archer = genie::EffectCommand();
    ec_archer.Type =
        genie::EffectCommand::EffectType::RelativeAttributeModifier;
    ec_archer.TargetUnit = unit_id;
    ec_archer.UnitClassID = -1;
    ec_archer.AttributeID = genie::EffectCommand::Attributes::Attack;
    ec_archer.Amount = building_attack_base + 1;
    ec_sara_2.push_back(ec_archer);
  }

  // Creates tech and effect for the Castle Age.
  const int effect_id_saracens_castle = aocDat->Effects.size();
  const int tech_id_saracens_castle = aocDat->Techs.size();

  auto tech_sara_3 = aocDat->Techs[tech_id_new];
  tech_sara_3.Name = "C 9, C-Bonus, Archers v Buildings 3";
  tech_sara_3.Name2 = "C 9, C-Bonus, Archers v Buildings 3";
  tech_sara_3.RequiredTechs[0] = 102; // Castle Age
  tech_sara_3.RequiredTechCount = 1;
  tech_sara_3.EffectID = effect_id_saracens_castle;
  tech_sara_3.Civ = civ_id_saracens;

  auto effect_sara_3 = aocDat->Effects[effect_id_new];
  effect_sara_3.Name = "C 9, C-Bonus, Archers v Buildings 3";
  auto& ec_sara_3 = effect_sara_3.EffectCommands;

  auto ec_sara_3_ca = genie::EffectCommand();
  ec_sara_3_ca.Type =
      genie::EffectCommand::EffectType::RelativeAttributeModifier;
  ec_sara_3_ca.TargetUnit = -1;
  ec_sara_3_ca.UnitClassID = 36;
  ec_sara_3_ca.AttributeID = genie::EffectCommand::Attributes::Attack;
  ec_sara_3_ca.Amount = building_attack_base + 1;
  ec_sara_3.push_back(ec_sara_3_ca);

  for (int unit_id : unit_id_archers) {
    auto ec_archer = genie::EffectCommand();
    ec_archer.Type =
        genie::EffectCommand::EffectType::RelativeAttributeModifier;
    ec_archer.TargetUnit = unit_id;
    ec_archer.UnitClassID = -1;
    ec_archer.AttributeID = genie::EffectCommand::Attributes::Attack;
    ec_archer.Amount = building_attack_base + 1;
    ec_sara_3.push_back(ec_archer);
  }

  aocDat->Effects.push_back(effect_sara_3);
  aocDat->Techs.push_back(tech_sara_3);

  // Creates tech and effect for the Imperial Age.
  const int effect_id_saracens_imp = aocDat->Effects.size();
  const int tech_id_saracens_imp = aocDat->Techs.size();

  auto tech_sara_4 = aocDat->Techs[tech_id_new];
  tech_sara_4.Name = "C 9, C-Bonus, Archers v Buildings 4";
  tech_sara_4.Name2 = "C 9, C-Bonus, Archers v Buildings 4";
  tech_sara_4.RequiredTechs[0] = 103; // Imperial Age
  tech_sara_4.RequiredTechCount = 1;
  tech_sara_4.EffectID = effect_id_saracens_imp;
  tech_sara_4.Civ = civ_id_saracens;

  auto effect_sara_4 = aocDat->Effects[effect_id_new];
  effect_sara_4.Name = "C 9, C-Bonus, Archers v Buildings 4";

  auto& ec_sara_4 = effect_sara_4.EffectCommands;

  auto ec_sara_4_ca = genie::EffectCommand();
  ec_sara_4_ca.Type =
      genie::EffectCommand::EffectType::RelativeAttributeModifier;
  ec_sara_4_ca.TargetUnit = -1;
  ec_sara_4_ca.UnitClassID = 36;
  ec_sara_4_ca.AttributeID = genie::EffectCommand::Attributes::Attack;
  ec_sara_4_ca.Amount = building_attack_base + 1;
  ec_sara_4.push_back(ec_sara_4_ca);

  for (int unit_id : unit_id_archers) {
    auto ec_archer = genie::EffectCommand();
    ec_archer.Type =
        genie::EffectCommand::EffectType::RelativeAttributeModifier;
    ec_archer.TargetUnit = unit_id;
    ec_archer.UnitClassID = -1;
    ec_archer.AttributeID = genie::EffectCommand::Attributes::Attack;
    ec_archer.Amount = building_attack_base + 1;
    ec_sara_4.push_back(ec_archer);
  }

  aocDat->Effects.push_back(effect_sara_4);
  aocDat->Techs.push_back(tech_sara_4);

  // Sets Saracen Market Cost to 75w (175w - 100 w)
  const int effect_id_saracen_market = 354;
  auto& effect_commands_saracen_market =
      aocDat->Effects[effect_id_saracen_market].EffectCommands;
  effect_commands_saracen_market[1].Amount = -100;
  effect_commands_saracen_market[2].Amount = -100;
  effect_commands_saracen_market[3].Amount = -100;

  // Sets Mameluke Frame Delay to 4 and Elite Mameluke Frame Delay to 2
  const int unit_id_mameluke = 282;
  const int unit_id_elite_mameluke = 556;
  for (auto& civ : aocDat->Civs) {
    civ.Units[unit_id_mameluke].Combat.FrameDelay = 4;
    civ.Units[unit_id_elite_mameluke].Combat.FrameDelay = 2;
  }

  // Sets Halberdier attack vs Class 35 to 11
  const int unit_id_halberdier = 359;
  for (auto& civ : aocDat->Civs) {
    civ.Units[unit_id_halberdier].Combat.Attacks[7].Amount = 11;
  }

  // Gives the Cavalry Archer armor class to Conquistadors and Elite
  // Conquistadors.
  const int unit_id_conquistador = 771;
  const int unit_id_elite_conquistador = 773;
  const int cav_archer_armor_class = 28;
  for (auto& civ : aocDat->Civs) {
    // Duplicates the final armor class, then sets its class to cav archer
    // armor.
    auto& armors_conq = civ.Units[unit_id_conquistador].Combat.Armours;
    armors_conq.push_back(armors_conq[5]);
    armors_conq[6].Class = cav_archer_armor_class;
    auto& armors_conq_elite =
        civ.Units[unit_id_elite_conquistador].Combat.Armours;
    armors_conq_elite.push_back(armors_conq_elite[5]);
    armors_conq_elite[6].Class = cav_archer_armor_class;
  }

  // Sets Missionary Heal Rate to 2 (so it equals that of monks).
  const int unit_id_missionary = 775;
  // The Heal task is at index 0.
  // Changes Work Value 1 from 1 to 2.
  aocDat->UnitHeaders[unit_id_missionary].TaskList[0].WorkValue1 = 2;

  // Makes Husbandry apply to Missionaries.
  const int effect_id_husbandry = 39;
  auto& effect_commands_husbandry =
      aocDat->Effects[effect_id_husbandry].EffectCommands;
  auto speed_multiplier_ec = effect_commands_husbandry[0];
  speed_multiplier_ec.TargetUnit = unit_id_missionary;
  speed_multiplier_ec.UnitClassID = -1;
  effect_commands_husbandry.push_back(speed_multiplier_ec);

  // Gives Slavs free Supplies upon reaching the Feudal Age.
  // TODO implement

  auto tech_id_supplies_free = aocDat->Techs.size();
  auto effect_id_supplies_free = aocDat->Effects.size();

  auto effect_supplies_free = aocDat->Effects[effect_id_new];
  effect_supplies_free.Name = "Free Slav Supplies";
  // Sets research time to 0.
  auto supplies_free_time = genie::EffectCommand();
  supplies_free_time.Type = 8;
  supplies_free_time.TargetUnit = effect_id_supplies;
  supplies_free_time.UnitClassID = -1;
  supplies_free_time.AttributeID = -1;
  supplies_free_time.Amount = 0;
  effect_supplies_free.EffectCommands.push_back(supplies_free_time);
  // Sets food cost to 0.
  auto supplies_free_food = genie::EffectCommand();
  supplies_free_food.Type =
      genie::EffectCommand::EffectCommand::TechCostModifier;
  supplies_free_food.TargetUnit = tech_id_supplies_free;
  supplies_free_food.UnitClassID = 0;
  supplies_free_food.AttributeID = 0;
  supplies_free_food.Amount = 0;
  effect_supplies_free.EffectCommands.push_back(supplies_free_food);
  aocDat->Effects.push_back(effect_supplies_free);
  // Sets gold cost to 0.
  auto supplies_free_gold = genie::EffectCommand();
  supplies_free_gold.Type =
      genie::EffectCommand::EffectCommand::TechCostModifier;
  supplies_free_gold.TargetUnit = tech_id_supplies_free;
  supplies_free_gold.UnitClassID = 3;
  supplies_free_gold.AttributeID = 0;
  supplies_free_gold.Amount = 0;
  effect_supplies_free.EffectCommands.push_back(supplies_free_gold);
  aocDat->Effects.push_back(effect_supplies_free);

  auto tech_supplies_free = aocDat->Techs[tech_id_new];
  tech_supplies_free.Name = "Free Slav Supplies";
  tech_supplies_free.Name2 = "Free Slav Supplies";
  tech_supplies_free.RequiredTechs[0] = 101;
  tech_supplies_free.RequiredTechCount = 1;
  tech_supplies_free.EffectID = effect_id_supplies_free;
  tech_supplies_free.Civ = 23; // Slavs
  aocDat->Techs.push_back(tech_supplies_free);

  // Sets Slav farming bonus to 10%.
  const int effect_id_slavs_tech_tree = 7;
  auto& effect_commands_slav =
      aocDat->Effects[effect_id_slavs_tech_tree].EffectCommands;
  effect_commands_slav[15].Amount = 1.18f;
  effect_commands_slav[16].Amount = 1.18f;
  effect_commands_slav[30].Amount = 1.1f;
  effect_commands_slav[31].Amount = 1.1f;

  // Enables Faith for Slavs (execute after changing the other effect commands).
  effect_commands_slav.erase(effect_commands_slav.begin() + 1);

  // Sets Boyar and Elite Boyar training time to 15s.
  const int unit_id_boyar = 876;
  const int unit_id_elite_boyar = 878;
  for (auto& civ : aocDat->Civs) {
    civ.Units[unit_id_boyar].Creatable.TrainTime = 15;
    civ.Units[unit_id_elite_boyar].Creatable.TrainTime = 15;
  }

  // Gives Teutons free Herbal Medicine.
  auto tech_id_herbal_medicine_free = aocDat->Techs.size();
  auto effect_id_herbal_medicine_free = aocDat->Effects.size();

  auto effect_herbal_medicine_free = aocDat->Effects[effect_id_new];
  effect_herbal_medicine_free.Name = "Free Teuton Herbal Medicine";
  // Sets research time to 0.
  auto herbal_medicine_free_time = genie::EffectCommand();
  herbal_medicine_free_time.Type = 8;
  herbal_medicine_free_time.TargetUnit = effect_id_herbal_medicine;
  herbal_medicine_free_time.UnitClassID = -1;
  herbal_medicine_free_time.AttributeID = -1;
  herbal_medicine_free_time.Amount = 0;
  effect_herbal_medicine_free.EffectCommands.push_back(
      herbal_medicine_free_time);
  // Sets gold cost to 0.
  auto herbal_medicine_free_gold = genie::EffectCommand();
  herbal_medicine_free_gold.Type =
      genie::EffectCommand::EffectCommand::TechCostModifier;
  herbal_medicine_free_gold.TargetUnit = tech_id_herbal_medicine_free;
  herbal_medicine_free_gold.UnitClassID = 3;
  herbal_medicine_free_gold.AttributeID = 0;
  herbal_medicine_free_gold.Amount = 0;
  effect_herbal_medicine_free.EffectCommands.push_back(
      herbal_medicine_free_gold);
  aocDat->Effects.push_back(effect_herbal_medicine_free);

  auto tech_herbal_medicine_free = aocDat->Techs[tech_id_new];
  tech_herbal_medicine_free.Name = "Free Teuton Herbal Medicine";
  tech_herbal_medicine_free.Name2 = "Free Teuton Herbal Medicine";
  tech_herbal_medicine_free.RequiredTechs[0] = 102;
  tech_herbal_medicine_free.RequiredTechCount = 1;
  tech_herbal_medicine_free.EffectID = effect_id_herbal_medicine_free;
  tech_herbal_medicine_free.Civ = 4;
  aocDat->Techs.push_back(tech_herbal_medicine_free);

  // Makes Vietnamese receive Husbandry
  const int effect_id_vietnamese_tech_tree = 652;
  auto& effect_commands_viet =
      aocDat->Effects[effect_id_vietnamese_tech_tree].EffectCommands;
  effect_commands_viet.erase(effect_commands_viet.begin() + 3);

  // Sets Paper Money cost to 500f/300w and research time to 60s.
  const int tech_id_paper_money = 629;
  aocDat->Techs[tech_id_paper_money].ResearchTime = 60;
  aocDat->Techs[tech_id_paper_money].ResourceCosts[0].Amount = 500;
  aocDat->Techs[tech_id_paper_money].ResourceCosts[1].Type = 1;
  aocDat->Techs[tech_id_paper_money].ResourceCosts[1].Amount = 300;
}

DatPatch balance = {&balancePatch, "Applies balance changes."};

} // namespace wololo
