# Game Planning 2025-08-06

## 1. Ideas & Concepts

### 1.1 Game Title

- Abyss
- Apothic: lightless, dark
- Deimos: feelings of dread and terror that befell those before a battle

### 1.2 Themes

- Dark Fantasy: "post apocalyptic" fantasy world overrun with baddies (undead, daemons, orcs, etc.)
- Survival: think factorio death world but less focus on factory and more on units/battles
- Goal: Banish evil, build a kingdom
- Sandbox: have fun doing what you want

### 1.3 Gameplay

- Grow: create your character, level up, and become a hero
- Explore: find treasures and lost ruins in an open world
- Mine: aquire resources
- Craft: make items to aid your cause
- Build: bases and recruit an army
- Survive: waves of enemies at night
- Fight: attack enemy spawn locations

### 1.4 World

- Grid/tile based (orthoganal or isometric)
- Zoom out to map view like factorio

Day & Night cycle

- Morning: 1 min, dark to light
- Day: 9 min, more peaceful
- Evening: 1 min, light to dark
- Night: 9 min, factorio deathworld style

Weather

- Normal
- Precipitation

Biomes

- Forest

Resources

- Soul: enemies
- Food: berries, animals, farmables
- Wood: trees
- Stone: stone
- Coal:
- Iron:
- Gold:
- Others: dirt, water, gems, crystal, oil, obsidian, tin copper, bronze, silver, flint, wool

### 1.5 Character

- Customizabble aesthetics

Level up skills by doing things in game (0 to 255)

- Strength: higher base melee damage as you hit baddies in melee
- Agility: higher character base speed as you move
- Constitution: higher base resistances as you get hit?
- Dexterity: higher base range damage as you hit baddies at range
- Magic: higher base mana as you use spells
- Mining: faster mining speed and more chance to get extra resources?
- Stealth?: sneaky
- Luck?: finding treasure
- Wisdom: greater max population
- Leadership: better modifiers on unit formations
- Charisma: better trade deals?

### 1.6 Entities

Units

- Formations
- Formation bonuses
- Varients: melee, range, magic, artillery, cavalry

Units - Friendly

- Spawn at cities and eventually can summon around character

Units - Enemies

- Spawn at dark shrines/altars
- Enemies amass and attack at night and retreat/despawn during day
- Different types: undead, orcs/goblins, beastmen, etc.
- Purging dark shrines can only happen at night and should be difficult but rewarding

Units - Groups

- Multiple units selected automatically become a group like AoE2
- Get a flag overhead like twwh so you can select group
- Groups can select formations (like aoe) that provide stat/game bonuses

Formations include:

- Line: buff defense
- Wedge: buff offense
- Column: buff speed, debuff defense

Units - Control

- Semi automated AI with standard commands?
- Default to attack move instead of just move like Dune 2 The Golden Path Mod?

### 1.7 Inspirations

- Factorio
- Minecraft
- Warcraft
- Starcraft
- Age of Empires
- Terraria
- Total War Warhammer
- Heroes of Might and Magic
- Valheim
- Shining Force
- World Box

## 2. Architecture

### Project Structure

```plaintext
src
+-- audio
+-- core
+-- graphics
+-- ui
```

- Camera
- SpriteRenderer

### 2.1 Pipeline

1. Startup
1. Game Loop
    1. Input
    1. Update
    1. Render
1. Shutdown

### 2.2 Startup

### 2.3 Input

### 2.4 Update

### 2.5 Render

### 2.6 Shutdown