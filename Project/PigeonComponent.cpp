#include "PigeonComponent.h"
#include "GameObject.h"
#include <math.h>
#include "Direct3D.h"
#include "Scene.h"
#include "FirstPersonPlayer.h"

void PigeonComponent::SetPlayer(GameObject* player)
{
	m_pPlayer = player;
	FirstPersonPlayer* p = m_pPlayer->GetComponent<FirstPersonPlayer>();
	p->AddPigeon();
}

void PigeonComponent::Died()
{
	FirstPersonPlayer* p = m_pPlayer->GetComponent<FirstPersonPlayer>();
	p->RemovedPigeon();
}

void PigeonComponent::Update()
{
	if (m_pPlayer == nullptr)
		return;

	XMFLOAT3 posDifference = { m_pPlayer->GetWorldPosition().x - GetOwner()->GetWorldPosition().x,
								m_pPlayer->GetWorldPosition().y - GetOwner()->GetWorldPosition().y,
								m_pPlayer->GetWorldPosition().z - GetOwner()->GetWorldPosition().z };

	m_distanceToPlayer = abs(sqrt(pow(posDifference.x, 2) + pow(posDifference.z, 2)));
	//if (m_distanceToPlayer < ATTACKDISTANCE)
	//{
		if (m_pPlayer->GetWorldPosition().x != m_lastPlayerPos.x || m_pPlayer->GetWorldPosition().y != m_lastPlayerPos.y || m_pPlayer->GetWorldPosition().z != m_lastPlayerPos.z)
		{
			m_lastPlayerPos = { m_pPlayer->GetWorldPosition().x,m_pPlayer->GetWorldPosition().y,m_pPlayer->GetWorldPosition().z };
			m_curPath = Direct3D::GetInstance()->GetCurrentScene()->GetAStarPath(GetOwner()->GetWorldPosition(), m_pPlayer->GetWorldPosition());
		}
		
		if (m_curPath.size() > 0)
		{
			XMFLOAT3 targetDirection = {	m_curPath[0].x - GetOwner()->GetWorldPosition().x,
											m_curPath[0].y - GetOwner()->GetWorldPosition().y,
											m_curPath[0].z - GetOwner()->GetWorldPosition().z };
			
			float distance = abs(sqrt(pow(targetDirection.x, 2) + pow(targetDirection.z, 2)));
			targetDirection = { targetDirection.x / distance, targetDirection.y ,targetDirection.z / distance };
			

			if (distance > 0.5)
			{
				float heading = atan2(targetDirection.x, targetDirection.z) * radiansToDegrees;
				
				if (!isnan(heading)) 
				{
					GetOwner()->SetRotation({ 0, heading + 90, 0 });
				}
				
				XMFLOAT3 pos = GetOwner()->GetWorldPosition();

				pos.x -= GetOwner()->GetRight().x * 2 * Direct3D::GetInstance()->GetDeltaTime();
				pos.z -= GetOwner()->GetRight().z * 2 * Direct3D::GetInstance()->GetDeltaTime();

				GetOwner()->SetPosition(pos);
			}
			else
			{
				m_curPath.erase(m_curPath.begin());
			}
		}
	//}
}
