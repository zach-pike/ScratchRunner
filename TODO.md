# Blocks to implement

# General TODO
 - [ ] Add ImGui menu
 - [ ] Text rendering API for looks category

# Order of implementation
 2. [x] Variable 
 3. [x] List
 1. [ ] Operator
 2. [ ] Control
 4. [ ] Motion
 5. [ ] Looks
 6. [ ] Event

## Motion
 - [x] Move steps
 - [x] Turn left degrees
 - [x] Turn right degrees
 - [ ] Go to (field)
 - [x] Go to XY
 - [ ] Glide to
 - [ ] Glide to XY
 - [x] Point in direction 
 - [ ] Point towards
 - [ ] Change X by
 - [ ] Set X
 - [ ] Change Y by
 - [ ] Set Y
 - [ ] If on edge bounce
 - [ ] Set rotation style
 - [ ] X position Reporter
 - [ ] Y position Reporter
 - [ ] Direction Reporter


## Looks
 - [ ] Say for secs
 - [ ] Say
 - [ ] Think for secs
 - [ ] Think
 - [ ] Switch costume to
 - [ ] Next costume
 - [ ] Switch backdrop to
 - [ ] Switch backdrop and wait
 - [ ] Next backdrop
 - [ ] Change size by
 - [ ] Set size to
 - [ ] Change effects by
 - [ ] Set effect to
 - [ ] Clear graphics event
 - [ ] Show
 - [ ] Hide
 - [ ] Go to frontback
 - [ ] Go forward backward layers
 - [ ] Costume Reporter
 - [ ] Backdrop Reporter
 - [ ] Size Reporter

## Sound (Not being done currently)

## Event blocks
 - [ ] WhenFlagClicked
 - [ ] WhenKeyPressed
 - [ ] WhenSpriteClicked
 - [ ] WhenStageClicked
 - [ ] WhenBackdropSwitchesTo
 - [ ] WhenGreaterThan
 - [ ] WhenBroadcastRecv
 - [ ] Broadcast
 - [ ] Broadcast and wait

## Control
 - [ ] Wait
 - [ ] Repeat
 - [ ] Forever
 - [x] If
 - [x] Else If
 - [ ] Wait until
 - [ ] Repeat until
 - [ ] Stop
 - [ ] WhenStartAsClone
 - [ ] Create clone of
 - [ ] Delete this clone

## Sensing
 - [ ] Touching Reporter
 - [ ] Touching color Reporter
 - [ ] Color is touching color Reporter
 - [ ] Distance to Reporter
 - [ ] Ask and wait
 - [ ] Answer Reporter
 - [ ] Key pressed reporter
 - [ ] Mouse down reporter
 - [ ] Mouse X reporter
 - [ ] Mouse Y reporter
 - [ ] Set drag mode
 - [ ] Loudness Reporter
 - [ ] Timer Reporter
 - [ ] Reset timer
 - [ ] Of 
 - [ ] Current
 - [ ] Days since 2000
 - [ ] Username

## Operator blocks (All reporters)
 - [x] Add
 - [x] Subtract
 - [x] Multiply
 - [x] Divide
 - [x] Random
 - [x] Greater Than (B)
 - [x] Less Than (B)
 - [x] Equals (B)
 - [x] And (B)
 - [x] Or (B)
 - [x] Not (B)
 - [ ] Join
 - [ ] Letter of
 - [ ] Length of
 - [ ] Contains (B)
 - [ ] Mod
 - [ ] Round
 - [ ] operator_mathop

## Variable Blocks
 - [x] Variable value
 - [x] Set variable to
 - [x] Change variable by
 - [ ] Show variable (WIP wont be working on for awhile)
 - [ ] Hide variable (WIP wont be working on for awhile)

## List blocks
 - [x] List value
 - [x] Add to list
 - [x] Delete of list
 - [x] Delete all of
 - [x] Insert at list
 - [x] Replace item
 - [x] Item of list Reporter
 - [x] Item num of list Reporter
 - [x] Length of list Reporter
 - [x] List contains item Reporter (B)
 - [ ] Show list (WIP wont be working on for awhile)
 - [ ] Hide list (WIP wont be working on for awhile)