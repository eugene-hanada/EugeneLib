struct Buffer_t
{
    uint dspThrdId; //�X���b�hID���������� 
};
RWStructuredBuffer<uint> OutBuff : register(u0);

[numthreads(4, 4, 4)]
void main(uint3 dispatchThreadId : SV_DispatchThreadID)
{
    //��������ID���� 
    OutBuff[dispatchThreadId.x * 4 * 4 + dispatchThreadId.y * 4 +
    dispatchThreadId.z] =
    dispatchThreadId.x * 4 * 4 + dispatchThreadId.y * 4 + dispatchThreadId.z;
}