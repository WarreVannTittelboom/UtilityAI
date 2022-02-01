# Decision Making in Game AI: Utility AI
## Introduction
*In the world of game AI there exist various decision-making structures, among them is a technique called "Utility AI" or a "Utility System".* <br/>
###### The concept is quite simple:  <br/>
* When a decision needs to be made, the Utility AI system will gather data on the current situation. <br/>
* For every possible action a "utility score" will be calculated using the data gathered from this current situation. <br/>
* A behavior can then be selected based on the highest score or by using those scores- <br/>
to seed the probability distribution for a weighted random selection.<br/>
> If implemented correctly, this should result in the "best" behavior being selected for the current situation.<br/>

***Why is it called “Utility” AI?*** <br/>
The term “Utility” originates from Utilitarian ethics. In this theory, the morally correct action in a given situation is always determined to be the one that does the most good for the most people. <br/>
The “utility” of an action is a measurement of how much good it does.<br/>
More info on [Utilitarianism](https://youtu.be/-FrZl22_79Q).
## Simplified Example
### Example -> AI for winning Pokemon battles using Utility AI: <br/>
![Image_1](https://i.imgur.com/f7w0I2c.png)<br/>
##### Since we want our AI to win, we always want to pick the action that results in the highest probability of winning. <br/>
* Every turn the AI needs to choose between "FIGHT","BAG","POKEMON" or "RUN" <br/>
* Current HP-, type- and status data of the pokemon (and various other aspects) are used to score these actions. <br/>
* In this example we know from the gathered data that the player has no items or other Pokemon. <br/>
* Therefore, all scores except the one for "FIGHT" should be 0 in this situation, guaranteeing that "FIGHT" gets picked. <br/>

![Image_1](https://i.imgur.com/52QcUN4.png)<br/>
##### In this example the AI chose "FIGHT" and now it needs to pick the optimal move. <br/>
* The previously discussed data + the new data from the different moves is used for the scoring. <br/>
* Let's see how we could determine these scores for this situation <br/>
 ```
 Scoring Scale-> 1-5
 
 1.Tackle
 -Enemy low HP, likely does enough damage to defeat enemy => +++
 -EnemyType = fire, move = normal thus no resistance or weakness => +
 # Tackle Score = 4
 
 2.Growl
 -Lowers enemy attack, AI pokemon takes less damage => ++
 -Enemy low HP but move does no damage => -
 # Growl Score = 1
 
 3.Leech Seed
-Enemy already seeded, has no effect if already seeded => =0 
(use =0 if action should not be an option in given situation)
 # Leech Seed Score = 0

 4.Vine Whip
 -Enemy low HP, likely does enough damage to defeat enemy => +++
 -EnemyType = fire, move = grass thus deals half damage => -
 # Vine Whip Score = 2
 ```
 * The AI will use "Tackle" because it scored the highest in this situation. <br/>
 
 ### And... <br/> 
 ![Image_1](https://i.imgur.com/YVdXyO9.png)<br/>
 ###### GG no re <br/>
## Implementation
I originally planned to use this decision making structure as a replacement for a behavior tree in an existing project. <br/>
But after attempting to do so, I realised that correctly implementing Utility AI is a lot harder then understanding it. <br/>
This is why I decided to go with a simpler text based implementation instead of possibly ruining my existing project. <br/>
### Plan -> 2 Players fighting each other using various actions which are determined using Utility AI <br/> 
#### How do we define these players and their actions? <br/> 
##### Actions: <br/> 
* Attack -> Deal damage, usage: infinite <br/>
* Heavy Attack -> Deal more damage, usage: finite <br/>
* Bandage -> Restore a small amount of health, usage: infinite <br/>
* Heal -> Restore some health, usage: finite <br/>
##### Player: <br/> 
* Name -> Distinguish between diffrent players <br/>
* HP and MaxHP -> Current HP and Max possible HP of player <br/>
* MedKits and MaxMK -> Current amount of MK and Max possible MK of player (resource for Heal action) <br/>
* Heavy Attack Status -> Whether the heavy attack is available or not (bool) <br/>
#### Now we can make considerations using the player data (for scoring the actions later on) <br/> 
##### Scale: 0.f -> 1.f
* healthWeight -> playerHP / playerMaxHP (for correct Scale) <br/> 
* medKitCountWeight -> playerMK / playerMaxMK (for correct Scale) <br/> 
* medKitWeight -> if (playerMK > 0) = 1.f ! = 0.f (1 if player has medkits 0 if not) <br/> 
* heavyWeight -> if (heavystatus == true) = 1.f ! = 0.f (1 if player can use heavy attack 0 if not) <br/> 
### Usually curves are used to map utility <br/> 
#### Types of curves used in this project (Scale: 0.f -> 1.f) <br/> 
* Descending Linear Curve -> the lower a value, the higher the utility <br/>

![Image_1](https://i.imgur.com/scff1x2.png)<br/>
* Exponential Function -> for rapid increase/decrease of utility <br/>

![Image_1](https://i.imgur.com/gqYxdXK.png)<br/>
* Inverse/Asymptote Function -> 0.2/x -0.1 (does not respect scale on purpose) <br/>

![Image_1](https://i.imgur.com/41ZeClc.png)<br/>
### Finally we can design the "Reasoner" (brain of the players) <br/> 
#### Here we determine the final scores for each action and select which should be used <br/> 
##### heavy_score -> Plan: We always want to use heavy attack action if possbile <br/> 
* heavy_score = heavyWeight(player) -> guarantees highest score if true else lowest score <br/>
##### attack_score -> Plan: the lower HP the enemy is, the more usefull an attack is <br/>
* attack_score = desc_lin(c_healthWeight(enemy)) -> the lower the enemy is the higher score <br/>
but with use of minimum bound otherwise score could be 0 and attacking might not happen <br/>
##### heal_score -> Plan: Only heal if very low HP but should be scored highly if so <br/>
* heal_score = medKitWeight(player) * inverse(healthWeight(player)) -> medkitweight used to <br/>
check for MK's (similar to heavyscore) and health of player using inverse to increase extremly <br/>
increase utility the lower the player, using certain minimum bound (only use medkits when low) <br/>
##### bandage_score -> Plan: Only bandage if quite low HP but should be scored quite high if so <br/>
* bandage_score = exp(healthWeight(player)) -> health of player using desc exponential to rapidly <br/>
increase utility the lower the player, using certain minimum bound (only when quite low) <br/>
### With these final scores we should decide how to determine the correct action <br/>
#### -> By picking the action that got the highest score <br/>
#### OR
#### -> Using the scores to as weights for a weighted random selection <br/>
### Implementation Finished! <br/>
## Result
### Added randomness for testing -> extra MK's and heavys + player turns at random  <br/>
#### Let's take a look at highest score selection first <br/>
![Image_1](https://i.imgur.com/tkEor52.png)<br/>
* At the start the only actions taken are Attack and Heavy Attack. <br/>
* This makes sense because the other 2 actions have a min HP bound. <br/>

![Image_1](https://i.imgur.com/dQ7xkaj.png)<br/> 
* At this stage all diffrent actions are being used.
* We can see that the player currently losing takes more healing actions.
* Because attack score is quite low (enemy more HP) and heal score is high. 

![Image_1](https://i.imgur.com/iLTHRbF.png)<br/>
* Near the end of the fight, the only 2 actions taken are heavy attack and bandage. 
* This also makes sense since the players usually have used all med kits because <br/> 
once they are at the low HP bound the inverse graph used will give it a higher <br/> 
utility score then possible (breaking the scale) to guarantee them being used. 
* The bandage action always scores higher then the normal attack at this point.
#### Let's take a look at the weighted random selection now <br/>
![Image_1](https://i.imgur.com/SlY7pia.png)<br/>
* As expected, the result is similar but alot more random. <br/>
* The normal attack action is even used near the end of the game! <br/>
* In the highest score selection this never happens because the <br/>
attack cant score higher then the others but here the actions are <br/>
randomly selcted and the attack weight is quite high at this point. <br/>

## Conclusion
### Pros And Cons Of Utility AI
#### Pros:
* All actions are constantly being weight
* Handles unique situations quite nicely
* Allows for variation
#### Cons:
* Quite difficult to design, edit and maintain
* More fuzzy than binary
### Closing Thoughts
#### Utility AI can be a usefull tool that truely shines when used in combination with other decision making structures
#### Especially for complex systems in RPG/RTS/simulations projects
#### And lastly games that make good use of this technique 
* The Sims series -> Partly used for the decision making of the sims. <br/>
* Dragon Age: Inquisition -> Partly used for the companion behavior. <br/>
## References
[AI Architectures: A Culinary Guide](https://intrinsicalgorithm.com/IAonAI/2012/11/ai-architectures-a-culinary-guide-gdmag-article/) <br/>
[Are Behavior Trees a Thing of the Past?](https://www.gamedeveloper.com/programming/are-behavior-trees-a-thing-of-the-past-) <br/>
[How Utility AI Helps NPCs Decide What To Do Next | AI 101](https://www.youtube.com/watch?v=p3Jbp2cZg3Q) <br/>
[AI Made Easy with Utility AI](https://medium.com/@morganwalkupdev/ai-made-easy-with-utility-ai-fef94cd36161) <br/>
## Thank you for your time!

![Alt Text](https://c.tenor.com/WjoUFaID8ScAAAAC/cat-cute.gif)

