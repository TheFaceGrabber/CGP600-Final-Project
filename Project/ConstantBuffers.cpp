#include "ConstantBuffers.h"
#include "Direct3D.h"

ConstantBuffers* ConstantBuffers::m_pInstance = NULL;

ConstantBuffers::ConstantBuffers()
{
	for (int i = 0; i < BUFFER_COUNT; i++)
	{
		D3D11_BUFFER_DESC constant_buffer_desc;
		ZeroMemory(&constant_buffer_desc, sizeof(constant_buffer_desc));
		constant_buffer_desc.Usage = D3D11_USAGE_DEFAULT;

		if(i == BUFFER_LIGHTING)
			constant_buffer_desc.ByteWidth = sizeof(LightingBuffer);
		else if(i == BUFFER_TRANSFORMATIONS)
			constant_buffer_desc.ByteWidth = sizeof(TransformationBuffer);
		else if (i == BUFFER_SHADER_PROPERTIES)
			constant_buffer_desc.ByteWidth = BUFFER_SHADER_PROPERTIES_MAX_SIZE;

		constant_buffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

		Direct3D::GetInstance()->GetDevice()->CreateBuffer(&constant_buffer_desc, NULL, &m_pBuffers[i]);
	}
}

ConstantBuffers::~ConstantBuffers()
{
	for (int i = 0; i < BUFFER_COUNT; i++)
	{
		if(m_pBuffers[i])
			m_pBuffers[i]->Release();
	}
}

ConstantBuffers* ConstantBuffers::GetInstance()
{
	if (!m_pInstance)
	{
		m_pInstance = new ConstantBuffers();
	}

	return m_pInstance;
}


void ConstantBuffers::Bind(int index, void* data)
{
	if (index < 0)
		return;

	Direct3D::GetInstance()->GetContext()->UpdateSubresource(m_pBuffers[index], 0, 0, data, 0, 0);
	Direct3D::GetInstance()->GetContext()->VSSetConstantBuffers(index, 1, &m_pBuffers[index]);
	Direct3D::GetInstance()->GetContext()->PSSetConstantBuffers(index, 1, &m_pBuffers[index]);
}
