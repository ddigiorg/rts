#

## Entities are per world not per chunk

The entities are no longer registered per-chunk when they are active but instead per-map. So there is no easy way to count entities per chunk; the game would have to go over every single entity that is active, check what chunk it is on, and then draw that for a given chunk. It would be quite slow and doesn't really matter since the per-chunk part of it is irrelevant now; the chunk isn't forced to be active just because an entity on it is active.

There are still some entities which register per-chunk (mainly turrets). So it still has some utility. But at the end of the day the debug features are there for us to use and if a player gets some use out of it too then sure why not.

https://forums.factorio.com/viewtopic.php?t=92566

## Factorio doubly linked circular list

We use our own implementation of a doubly linked circular intrusive list.

Updatable entities themselves are a node in the list so the node does not need to be allocated or deallocated when adding or removing the entity from a list.

Because it's a doubly linked list the entity can check in constant time if it is currently linked (does the pointer have a value) and can in constant time un-link itself (take previous and point at next, take next and point at previous, set my own values to nullptr)

Because the list is circular the 'end' is always the list itself which means you can add and remove while iterating and always know when you iterated to the end. The only caveat is; while updating you need to hold 'an' iterator; if the one you hold un-links itself it needs to advance the held iterator by 1. That isn't generally a problem since the list of active entities is known by everyone and can easily be checked before something goes inactive.

It has the downside of iterating being iteration over a linked list (next can not be pre-fetched without loading the current first) but due to each 'next' being the next thing to update it naturally loads in the next entity to update by simply iterating. My testing has shown it is no slower to iterate the linked list than iterate a vector of pointers; the time goes (10% load next | 90% update entity) or (1% load next, 99% update entity).

https://www.reddit.com/r/factorio/comments/13bsf3s/technical_questions/

# Factorio multiple inheritance

Entities are defined through inheritance. Things which are updatable will then multiple inherit from the base entity type as well as the updatable entity type. You can see some of the inheritance tree here in the prototype documentation: https://wiki.factorio.com/Prototype_definitions

https://www.reddit.com/r/factorio/comments/13bsf3s/technical_questions/

# Factorio prefetch next entity data

TODO

# Factorio tile collision masks

Collision masks are defined using a bitmask on a unsigned 16 bit integer. The 0 value is "none", the 0xFFFF value is "not defined" so that leaves 14 possible values for collision masks.

To make that bigger means every collision mask in the entire game is now bigger - even without mods using any of them.

I know there was talk of doing this but if we ever did increase it, it would only ever increase to a maximum of an unsinged 64 bit integer making for 62 possible collision masks.

https://forums.factorio.com/viewtopic.php?t=74786

# Factorio collision layers (old)

ground-tile
water-tile
resource-layer
floor-layer
item-layer
object-layer
player-layer
ghost-layer
doodat-layer

https://forums.factorio.com/viewtopic.php?t=2671

