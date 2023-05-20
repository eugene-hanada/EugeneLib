#include "../../Include/Math/Quaternion.h"
#include "../../Include/ThirdParty/DirectXMath/DirectXMath.h"
#include "../../Include/Math/Geometry.h"

Eugene::Quaternion::Quaternion(float rotX, float rotY, float rotZ)
{
    DirectX::XMFLOAT4 q;
    DirectX::XMStoreFloat4(&q,DirectX::XMQuaternionRotationRollPitchYaw(rotX, rotY, rotZ));
    x = q.x;
    y = q.y;
    z = q.z;
    w = q.w;
}

Eugene::Quaternion::Quaternion(const Matrix4x4& matrix)
{
    DirectX::XMFLOAT4 q;
    DirectX::XMStoreFloat4(&q,DirectX::XMQuaternionRotationMatrix(DirectX::XMLoadFloat4x4(&matrix)));
    x = q.x;
    y = q.y;
    z = q.z;
    w = q.w;
}

const float Eugene::Quaternion::Magnitude(void) const
{
    DirectX::XMFLOAT4 q{ x,y,z,w };
    return DirectX::XMQuaternionLength(DirectX::XMLoadFloat4(&q)).m128_f32[0];
}

const float Eugene::Quaternion::SqMagnitude(void) const
{
    DirectX::XMFLOAT4 q{ x,y,z,w };
    return DirectX::XMQuaternionLengthSq(DirectX::XMLoadFloat4(&q)).m128_f32[0];
}

void Eugene::Quaternion::Normalize(void)
{
    DirectX::XMFLOAT4 q{ x,y,z,w };
    DirectX::XMStoreFloat4(&q, DirectX::XMQuaternionNormalize(DirectX::XMLoadFloat4(&q)));
    x = q.x;
    y = q.y;
    z = q.z;
    w = q.w;
}

Eugene::Quaternion Eugene::Quaternion::Normalized(void) const
{
    Quaternion q = *this;
    q.Normalize();
    return q;
}

Eugene::Vector3 Eugene::Quaternion::ToEuler(void) const
{
    double ww = w * w;
    double  wx = w * x;
    double  wy = w * y;
    double  wz = w * z;
    double  xx = x * x;
    double  xy = x * y;
    double  xz = x * z;
    double  yy = y * y;
    double  yz = y * z;
    double  zz = z * z;
    return {
        static_cast<float>(std::asin(std::clamp(-2.0 * (yz - wx), -1.0, 1.0))),
        static_cast<float>(std::atan2((xz + wy) * 2.0, ww - xx - yy + zz)),
        static_cast<float>(std::atan2((xy + wz) * 2.0,ww - xx + yy - zz))
    };
}

void Eugene::Quaternion::ToMatrix(Matrix4x4& out) const
{
}

Eugene::Quaternion& Eugene::Quaternion::operator*=(const Quaternion& q)
{
    DirectX::XMFLOAT4 qA{ x,y,z,w };
    DirectX::XMFLOAT4 qB{ q.x,q.y,q.z,q.w };
    DirectX::XMStoreFloat4(&qA, DirectX::XMQuaternionMultiply(DirectX::XMLoadFloat4(&qA), DirectX::XMLoadFloat4(&qB)));
    x = qA.x;
    y = qA.y;
    z = qA.z;
    w = qA.w;
    return *this;
}

Eugene::Quaternion Eugene::operator*(const Quaternion& qL, const Quaternion& qR)
{
    DirectX::XMFLOAT4 r{ qR.x,qR.y,qR.z,qR.w };
    DirectX::XMFLOAT4 l{ qL.x,qL.y,qL.z,qL.w };
    DirectX::XMStoreFloat4(&r, DirectX::XMQuaternionMultiply(DirectX::XMLoadFloat4(&l), DirectX::XMLoadFloat4(&r)));
    return Quaternion{r.x, r.y, r.z, r.w};
}

Eugene::Vector3 Eugene::operator*(const Quaternion& q, const Vector3& vec)
{
    DirectX::XMFLOAT4 qf{ q.x,q.y,q.z,q.w };
    DirectX::XMFLOAT3 vf{ vec.x,vec.y, vec.z };
    DirectX::XMStoreFloat3(&vf,DirectX::XMVector3Rotate(DirectX::XMLoadFloat3(&vf), DirectX::XMLoadFloat4(&qf)));
    return Vector3{vf.x, vf.y, vf.z};
}
