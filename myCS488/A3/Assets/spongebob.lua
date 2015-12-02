rootnode = gr.node('root')
rootnode:rotate('y', -20.0)
rootnode:scale( 0.25, 0.25, 0.25 )
rootnode:translate(0.0, 0.0, -1.0)

red = gr.material({1.0, 0.0, 0.0}, {0.1, 0.1, 0.1}, 10)
blue = gr.material({0.0, 0.0, 1.0}, {0.1, 0.1, 0.1}, 10)
green = gr.material({0.0, 1.0, 0.0}, {0.1, 0.1, 0.1}, 10)
white = gr.material({1.0, 1.0, 1.0}, {0.1, 0.1, 0.1}, 10)
black = gr.material({0.0, 0.0, 0.0}, {0.1, 0.1, 0.1}, 10)
bodyyellow = gr.material({1.0, 0.968, 0.37}, {0.1, 0.1, 0.1}, 10)
iris = gr.material({0.235, 0.705, 0.827}, {0.1, 0.1, 0.1}, 10)
pantColour = gr.material({0.862, 0.572, 0.203}, {0, 0, 0}, 10)
grey = gr.material({0.329, 0.329, 0.329}, {0.1, 0.1, 0.1}, 10)

torso = gr.mesh('cube', 'torso')
rootnode:add_child(torso)
torso:set_material(bodyyellow)
torso:scale(1.0,0.9,0.33);

noseJoint = gr.joint('noseJoint', {-3,0,3}, {-3,0,3})
torso:add_child(noseJoint)

nose = gr.mesh('sphere', 'nose')
nose:set_material(bodyyellow)
nose:scale(0.05,0.05,0.60)
nose:translate(0,0,0.8)
noseJoint:add_child(nose)

--setup eye balls

leftEyeBall = gr.mesh('sphere', 'leftEyeBall')
leftEyeBall:set_material(white)
leftEyeBall:scale(0.13,0.1,0.5)
leftEyeBall:translate(0.25,0.25,0.35)
torso:add_child(leftEyeBall)

rightEyeBall = gr.mesh('sphere', 'rightEyeBall')
rightEyeBall:set_material(white)
rightEyeBall:scale(0.13,0.1,0.5)
rightEyeBall:translate(-0.25,0.25,0.35)
torso:add_child(rightEyeBall)

--setup iris

leftIris = gr.mesh('sphere', 'leftIris')
leftIris:set_material(iris)
leftIris:scale(0.45,0.45,0.25)
leftIris:translate(0,0,0.8)
leftEyeBall:add_child(leftIris)

rightIris = gr.mesh('sphere', 'rightIris')
rightIris:set_material(iris)
rightIris:scale(0.45,0.45,0.25)
rightIris:translate(0,0,0.8)
rightEyeBall:add_child(rightIris)

--setup pupil

leftPupil = gr.mesh('sphere', 'leftPupil')
leftPupil:set_material(black)
leftPupil:scale(0.5,0.5,0.5)
leftPupil:translate(0,0,1)
leftIris:add_child(leftPupil)

rightPupil = gr.mesh('sphere', 'rightPupil')
rightPupil:set_material(black)
rightPupil:scale(0.5,0.5,0.5)
rightPupil:translate(0,0,1)
rightIris:add_child(rightPupil)

--setup left eye lashes

leftMiddleEyeLash = gr.mesh('cube', 'leftMiddleEyeLash')
leftMiddleEyeLash:set_material(black)
leftMiddleEyeLash:scale(0.2,0.60,0.1)
leftMiddleEyeLash:translate(0.0,1.1,0.3)
leftMiddleEyeLash:rotate('x', 10.0)
leftEyeBall:add_child(leftMiddleEyeLash)

leftLeftEyeLash = gr.mesh('cube', 'leftLeftEyeLash')
leftLeftEyeLash:set_material(black)
leftLeftEyeLash:scale(0.2,0.60,0.1)
leftLeftEyeLash:translate(0.0,1.1,0.3)
leftLeftEyeLash:rotate('x', 10.0)
leftLeftEyeLash:rotate('z', 30.0)
leftEyeBall:add_child(leftLeftEyeLash)


leftRightEyeLash = gr.mesh('cube', 'leftRightEyeLash')
leftRightEyeLash:set_material(black)
leftRightEyeLash:scale(0.2,0.60,0.1)
leftRightEyeLash:translate(0.0,1.1,0.3)
leftRightEyeLash:rotate('x', 10.0)
leftRightEyeLash:rotate('z', -30.0)
leftEyeBall:add_child(leftRightEyeLash)

--setup right eye lashes

rightMiddleEyeLash = gr.mesh('cube', 'rightMiddleEyeLash')
rightMiddleEyeLash:set_material(black)
rightMiddleEyeLash:scale(0.2,0.60,0.1)
rightMiddleEyeLash:translate(0.0,1.1,0.3)
rightMiddleEyeLash:rotate('x', 10.0)
rightEyeBall:add_child(rightMiddleEyeLash)

rightRightEyeLash = gr.mesh('cube', 'rightRightEyeLash')
rightRightEyeLash:set_material(black)
rightRightEyeLash:scale(0.2,0.60,0.1)
rightRightEyeLash:translate(0.0,1.1,0.3)
rightRightEyeLash:rotate('x', 10.0)
rightRightEyeLash:rotate('z', 30.0)
rightEyeBall:add_child(rightRightEyeLash)

rightLeftEyeLash = gr.mesh('cube', 'rightLeftEyeLash')
rightLeftEyeLash:set_material(black)
rightLeftEyeLash:scale(0.2,0.60,0.1)
rightLeftEyeLash:translate(0.0,1.1,0.3)
rightLeftEyeLash:rotate('x', 10.0)
rightLeftEyeLash:rotate('z', -30.0)
rightEyeBall:add_child(rightLeftEyeLash)

--setup mouth

middleLip = gr.mesh('cube', 'middleLip')
middleLip:set_material(black)
middleLip:scale(0.4,0.01,0.2)
middleLip:translate(0,-0.20,0.5)
torso:add_child(middleLip)

rightLip = gr.mesh('cube', 'rightLip')
rightLip:set_material(black)
rightLip:translate(-1.25,-27.0,2.5)
rightLip:scale(0.1,0.01,0.2);
rightLip:rotate('z', -30.0)
torso:add_child(rightLip)

leftLip = gr.mesh('cube', 'leftLip')
leftLip:set_material(black)
leftLip:translate(1.25,-27.0,2.5)
leftLip:scale(0.1,0.01,0.2);
leftLip:rotate('z', 30.0)
torso:add_child(leftLip)

teethGap = gr.mesh('cube', 'teethGap')
teethGap:set_material(black)
teethGap:scale(0.025,0.07,0.1)
teethGap:translate(0,-0.27,0.525)
torso:add_child(teethGap)

--setup teeth

teeth = gr.mesh('cube', 'teeth')
teeth:set_material(white)
teeth:scale(0.2,0.1,0.1)
teeth:translate(0,-0.255,0.5)
torso:add_child(teeth)


--setup shirt

shirt = gr.mesh('cube', 'shirt')
shirt:set_material(white)
shirt:translate(0,-8.5,0)
shirt:scale(1,0.0625,1)
torso:add_child(shirt)

--setup pants

pant = gr.mesh('cube', 'pant')
pant:set_material(pantColour)
pant:translate(0,-0.8,0)
pant:scale(1,1.6,1)
shirt:add_child(pant)

--setup shoulder

leftShoulderJoint = gr.joint('leftShoulderJoint', {0,0,0},{-8,0,8})
torso:add_child(leftShoulderJoint)

rightShoulderJoint = gr.joint('rightShoulderJoint', {0,0,0},{-8,0,8})
torso:add_child(rightShoulderJoint)

leftShoulder = gr.mesh('sphere', 'leftShoulder')
leftShoulder:set_material(white)
leftShoulder:translate(1.5,-6.5,0);
leftShoulder:scale(0.25,0.05,0.25);
leftShoulderJoint:add_child(leftShoulder)

rightShoulder = gr.mesh('sphere', 'rightShoulder')
rightShoulder:set_material(white)
rightShoulder:translate(-1.5,-6.5,0);
rightShoulder:scale(0.25,0.05,0.25);
rightShoulderJoint:add_child(rightShoulder)

leftUpperArm = gr.mesh('sphere','leftUpperArm')
leftUpperArm:set_material(bodyyellow)
leftUpperArm:translate(1.5,0,0)
leftUpperArm:scale(0.5,0.5,0.5)
leftShoulder:add_child(leftUpperArm)

rightUpperArm = gr.mesh('sphere','rightUpperArm')
rightUpperArm:set_material(bodyyellow)
rightUpperArm:translate(-1.5,0,0)
rightUpperArm:scale(0.5,0.5,0.5)
rightShoulder:add_child(rightUpperArm)

leftElbow = gr.mesh('sphere', 'leftElbow')
leftElbow:set_material(bodyyellow)
leftElbow:translate(3.25,0,0)
leftElbow:scale(0.25,1,0.5)
leftUpperArm:add_child(leftElbow)

rightElbow = gr.mesh('sphere', 'rightElbow')
rightElbow:set_material(bodyyellow)
rightElbow:translate(-3.25,0,0)
rightElbow:scale(0.25,1,0.5)
rightUpperArm:add_child(rightElbow)

leftElbowJoint=gr.joint('leftElbowJoint', {0,0,0},{-10,0,10})
leftUpperArm:add_child(leftElbowJoint)

rightElbowJoint=gr.joint('rightElbowJoint', {0,0,0},{-10,0,10})
rightUpperArm:add_child(rightElbowJoint)

leftLowerArm=gr.mesh('sphere','leftLowerArm')
leftLowerArm:set_material(bodyyellow)
leftLowerArm:translate(1.9,0,0)
leftElbowJoint:add_child(leftLowerArm)

rightLowerArm=gr.mesh('sphere','rightLowerArm')
rightLowerArm:set_material(bodyyellow)
rightLowerArm:translate(-1.9,0,0)
rightElbowJoint:add_child(rightLowerArm)

lefthandJoint = gr.joint('lefthandJoint', {-10,0,10}, {0,0,0})
leftLowerArm:add_child(lefthandJoint)

righthandJoint = gr.joint('righthandJoint', {-10,0,10}, {0,0,0})
rightLowerArm:add_child(righthandJoint)

leftHand = gr.mesh('sphere', 'leftHand')
leftHand:set_material(bodyyellow)
leftHand:translate(3,0,0)
leftHand:scale(0.25,0.75,0.5)
lefthandJoint:add_child(leftHand)

rightHand = gr.mesh('sphere', 'leftHand')
rightHand:set_material(bodyyellow)
rightHand:translate(-3,0,0)
rightHand:scale(0.25,0.75,0.5)
righthandJoint:add_child(rightHand)

leftStick = gr.mesh('cube', 'leftStick')
leftStick:set_material(black)
leftStick:scale(0.125,35,0.25)
leftStick:translate(0.85,15,0)
lefthandJoint:add_child(leftStick)

rightThumbJoint = gr.joint('rightThumbJoint', {-15,0,15}, {0,0,0})
rightHand:add_child(rightThumbJoint)

rightThumb = gr.mesh('sphere', 'rightThumb')
rightThumb:set_material(bodyyellow)
rightThumb:scale(0.4,2,0.4)
rightThumb:translate(-0.4,2,0)
rightThumbJoint:add_child(rightThumb)

rightIndexJoint = gr.joint('rightIndexJoint', {-10,0,10}, {0,0,0})
rightHand:add_child(rightIndexJoint)

rightIndex = gr.mesh('sphere', 'rightIndex')
rightIndex:set_material(bodyyellow)
rightIndex:scale(1,1/2,1/2)
rightIndex:translate(-0.99,0,0)
rightIndexJoint:add_child(rightIndex)

rightPinkyJoint = gr.joint('rightPinkyJoint', {-10,0,10}, {0,0,0})
rightHand:add_child(rightPinkyJoint)

rightPinky = gr.mesh('sphere', 'rightPinky')
rightPinky:set_material(bodyyellow)
rightPinky:scale(0.4,2,0.4)
rightPinky:translate(-0.4,-2,0)
rightPinkyJoint:add_child(rightPinky)

-- pant detail

pantDetail1 = gr.mesh('cube', 'pantDetail1')
pantDetail1:set_material(black)
pantDetail1:translate(-2.2,0.9,1.75)
pantDetail1:scale(0.15,0.25,1/4)
pant:add_child(pantDetail1)

pantDetail2 = gr.mesh('cube', 'pantDetail2')
pantDetail2:set_material(black)
pantDetail2:translate(-0.7,0.9,1.75)
pantDetail2:scale(0.15,0.25,1/4)
pant:add_child(pantDetail2)

pantDetail3 = gr.mesh('cube', 'pantDetail3')
pantDetail3:set_material(black)
pantDetail3:translate(0.8,0.9,1.75)
pantDetail3:scale(0.15,0.25,1/4)
pant:add_child(pantDetail3)

pantDetail4 = gr.mesh('cube', 'pantDetail4')
pantDetail4:set_material(black)
pantDetail4:translate(2.3,0.9,1.75)
pantDetail4:scale(0.15,0.25,1/4)
pant:add_child(pantDetail4)

--setup leg

leftHipJoint = gr.joint('leftHipJoint', {-20,0,30}, {0,0,0})
torso:add_child(leftHipJoint)

rightHipJoint = gr.joint('rightHipJoint', {-20,0,30}, {0,0,0})
torso:add_child(rightHipJoint)

leftPantSleeve = gr.mesh('cube', 'leftPantSleeve')
leftPantSleeve:set_material(pantColour)
leftPantSleeve:scale(0.15,0.15,0.3)
leftPantSleeve:translate(-0.25,-0.7,0)
leftHipJoint:add_child(leftPantSleeve)

rightPantSleeve = gr.mesh('cube', 'rightPantSleeve')
rightPantSleeve:set_material(pantColour)
rightPantSleeve:scale(0.15,0.15,0.3)
rightPantSleeve:translate(0.25,-0.7,0)
rightHipJoint:add_child(rightPantSleeve)

leftThigh = gr.mesh('cube', 'leftThigh')
leftThigh:set_material(bodyyellow)
leftThigh:scale(0.5,1.5,0.9)
leftThigh:translate(0,-1,0)
leftPantSleeve:add_child(leftThigh)

rightThigh = gr.mesh('cube', 'rightThigh')
rightThigh:set_material(bodyyellow)
rightThigh:scale(0.5,1.5,0.9)
rightThigh:translate(0,-1,0)
rightPantSleeve:add_child(rightThigh)

leftKneecap = gr.mesh('sphere', 'leftKneecap')
leftKneecap:set_material(bodyyellow)
leftKneecap:scale(0.75,0.2,0.75)
leftKneecap:translate(0,-0.5,0)
leftThigh:add_child(leftKneecap)

rightKneecap = gr.mesh('sphere', 'rightKneecap')
rightKneecap:set_material(bodyyellow)
rightKneecap:scale(0.75,0.2,0.75)
rightKneecap:translate(0,-0.5,0)
rightThigh:add_child(rightKneecap)

leftKneecapJoint = gr.joint('leftKneecapJoint', {-30,0,30}, {0,0,0})
leftKneecap:add_child(leftKneecapJoint)

rightKneecapJoint = gr.joint('rightKneecapJoint', {-30,0,30}, {0,0,0})
rightKneecap:add_child(rightKneecapJoint)

leftLeg = gr.mesh('cube', 'leftLeg')
leftLeg:set_material(bodyyellow)
leftLeg:scale(1,4,1)
leftLeg:translate(0,-2.0,0)
leftKneecapJoint:add_child(leftLeg)

rightLeg = gr.mesh('cube', 'rightLeg')
rightLeg:set_material(bodyyellow)
rightLeg:scale(1,4,1)
rightLeg:translate(0,-2.0,0)
rightKneecapJoint:add_child(rightLeg)

leftSock = gr.mesh('cube','leftSock')
leftSock:translate(0,-0.5,0)
leftSock:scale(1.05,0.8,1.05)
leftSock:set_material(white)
leftLeg:add_child(leftSock)

rightSock = gr.mesh('cube','rightSock')
rightSock:translate(0,-0.5,0)
rightSock:scale(1.05,0.8,1.05)
rightSock:set_material(white)
rightLeg:add_child(rightSock)

leftSockDetail1 = gr.mesh('cube', 'leftSockDetail1')
leftSockDetail1:set_material(blue)
leftSockDetail1:scale(1.05,0.1,1.05)
leftSockDetail1:translate(0,0.2,0)
leftSock:add_child(leftSockDetail1)

rightSockDetail1 = gr.mesh('cube', 'rightSockDetail1')
rightSockDetail1:set_material(blue)
rightSockDetail1:scale(1.05,0.1,1.05)
rightSockDetail1:translate(0,0.2,0)
rightSock:add_child(rightSockDetail1)

leftSockDetail2 = gr.mesh('cube', 'leftSockDetail2')
leftSockDetail2:set_material(red)
leftSockDetail2:scale(1.05,0.1,1.05)
leftSockDetail2:translate(0,0.05,0)
leftSock:add_child(leftSockDetail2)

rightSockDetail2 = gr.mesh('cube', 'rightSockDetail2')
rightSockDetail2:set_material(red)
rightSockDetail2:scale(1.05,0.1,1.05)
rightSockDetail2:translate(0,0.05,0)
rightSock:add_child(rightSockDetail2)

leftAnkle = gr.joint('leftAnkle', {-10,0,10}, {-5,-5,5})
leftSock:add_child(leftAnkle)

rightAnkle = gr.joint('rightAnkle', {-10,0,10}, {-5,-5,5})
rightSock:add_child(rightAnkle)

leftBackShoe = gr.mesh('cube','leftBackShoe')
leftBackShoe:set_material(black)
leftBackShoe:scale(1.2,0.5,1.2)
leftBackShoe:translate(0,-0.5,0)
leftAnkle:add_child(leftBackShoe)

rightBackShoe = gr.mesh('cube','rightBackShoe')
rightBackShoe:set_material(black)
rightBackShoe:scale(1.2,0.5,1.2)
rightBackShoe:translate(0,-0.5,0)
rightAnkle:add_child(rightBackShoe)

leftFrontShoe = gr.mesh('cube', 'leftFrontShoe')
leftFrontShoe:set_material(black)
leftFrontShoe:scale(1,0.4,1)
leftFrontShoe:translate(0,-0.3,1)
leftBackShoe:add_child(leftFrontShoe)

rightFrontShoe = gr.mesh('cube', 'rightFrontShoe')
rightFrontShoe:set_material(black)
rightFrontShoe:scale(1,0.4,1)
rightFrontShoe:translate(0,-0.3,1)
rightBackShoe:add_child(rightFrontShoe)

return rootnode