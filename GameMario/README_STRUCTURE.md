# GameMario Project Structure


```
GameMario/
├── src/
│   ├── core/                    # Game core & lifecycle
│   │   ├── Game.h/.cpp         # Main game class
│   │   ├── main.cpp            # Entry point
│   │   ├── GameState.h/.cpp    # Game state management
│   │   ├── Scene.h             # Base scene class
│   │   ├── PlayScene.h/.cpp    # Main play scene
│   │   └── GameObject.h/.cpp   # Base game object
│   │
│   ├── engine/                  # Engine-level systems
│   │   ├── rendering/          # Rendering & graphics
│   │   │   ├── Texture.h       # Texture definition
│   │   │   ├── Textures.h/.cpp # Texture manager
│   │   │   ├── Sprite.h/.cpp   # Sprite class
│   │   │   ├── Sprites.h/.cpp  # Sprite manager
│   │   │   ├── Animation*.h/.cpp # Animation system
│   │   │   └── Particle.h/.cpp # Particle effects
│   │   │
│   │   ├── physics/            # Physics & collision
│   │   │   └── Collision.h/.cpp # Collision detection
│   │   │
│   │   └── input/              # Input handling
│   │       ├── KeyEventHandler.h
│   │       └── SampleKeyEventHandler.h/.cpp
│   │
│   ├── entities/               # Game entities
│   │   ├── player/            # Player-related
│   │   │   ├── Mario.h/.cpp   # Main player
│   │   │   ├── TailWhip.h/.cpp # Tail attack
│   │   │   ├── Fireball.h/.cpp # Fire attack
│   │   │   └── Boomerang.h/.cpp # Boomerang attack
│   │   │
│   │   ├── enemies/           # Enemy entities
│   │   │   ├── Goomba.h/.cpp  # Basic enemy
│   │   │   ├── WingedGoomba.h/.cpp
│   │   │   ├── Koopa.h/.cpp   # Shell enemy
│   │   │   ├── WingedKoopa.h/.cpp
│   │   │   ├── FlyingKoopa.h/.cpp
│   │   │   ├── PiranhaPlant.h/.cpp
│   │   │   └── BoomerangTurtle.h/.cpp
│   │   │
│   │   ├── items/             # Collectible items
│   │   │   ├── Mushroom.h/.cpp    # Power mushroom
│   │   │   ├── LifeMushroom.h/.cpp # 1-up mushroom
│   │   │   ├── Coin.h/.cpp        # Coins
│   │   │   ├── SuperLeaf.h/.cpp   # Raccoon power
│   │   │   └── Trinket.h/.cpp     # Special items
│   │   │
│   │   └── blocks/            # Interactive blocks
│   │       ├── Brick.h/.cpp       # Breakable bricks
│   │       ├── CoinBrick.h/.cpp   # Coin containing bricks
│   │       ├── HiddenCoinBrick.h/.cpp
│   │       ├── LifeBrick.h/.cpp   # 1-up bricks
│   │       ├── QuestionBlock.h/.cpp # ? blocks
│   │       ├── CoinQBlock.h/.cpp
│   │       ├── BuffQBlock.h/.cpp
│   │       ├── BuffRoulette.h/.cpp
│   │       ├── Box.h/.cpp
│   │       ├── Activator.h/.cpp
│   │       └── ActivatorBrick.h/.cpp
│   │
│   ├── world/                 # World & level objects
│   │   ├── Platform.h/.cpp    # Static platforms
│   │   ├── FallingPlatform.h/.cpp
│   │   ├── SkyPlatform.h/.cpp
│   │   ├── Pipe.h/.cpp        # Warp pipes
│   │   ├── Tree.h/.cpp        # Background trees
│   │   ├── Cloud.h/.cpp       # Background clouds
│   │   ├── Bush.h/.cpp        # Background bushes
│   │   ├── Border.h/.cpp      # Level boundaries
│   │   ├── Chunk.h/.cpp       # Level chunks
│   │   └── Portal.h/.cpp      # Level portals
│   │
│   ├── systems/              # Game-specific systems
│   │   ├── Utils.h/.cpp      # Utility functions
│   │   └── FallPitch.h/.cpp  # Fall/death handling
│   │
│   └── debug/                # Debug utilities
│       └── debug.h/.cpp      # Debug helpers
│
├── assets/                   # Asset definition files
├── scenes/                   # Scene configuration files
├── textures/                 # Texture files
└── Debug/                    # Build output