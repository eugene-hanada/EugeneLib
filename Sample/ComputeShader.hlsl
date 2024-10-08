struct Buffer_t
{
    uint dspThrdId; //スレッドIDを書き込む 
};
RWStructuredBuffer<uint> OutBuff : register(u0);

[numthreads(4, 4, 4)]
void main(uint3 dispatchThreadId : SV_DispatchThreadID)
{
    //ただただIDを代入 
    OutBuff[dispatchThreadId.x * 4 * 4 + dispatchThreadId.y * 4 +
    dispatchThreadId.z] =
    dispatchThreadId.x * 4 * 4 + dispatchThreadId.y * 4 + dispatchThreadId.z;
}