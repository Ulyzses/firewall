<script lang="ts">
  import { getModalStore } from '@skeletonlabs/skeleton';
  import type { ModalSettings, ModalComponent, ModalStore } from '@skeletonlabs/skeleton';
  const modalStore = getModalStore();

  const modalAddDevice: ModalSettings = {
    backdropClasses: '!backdrop-blur-md',
    modalClasses: '!bg-white',
    type: 'prompt',
    // Data
    title: 'Enter Device ID',
    body: 'Provide the MAC address of your device below, e.g. 202111579, 202112345',
    // Populates the input value and attributes
    value: '',
    valueAttr: { type: 'text', minlength: 9, maxlength: 9, required: true },
    // Returns the updated response value
    response: (r: string) => {
      addDeviceValue = r;
      console.log('response:', r);
    }
  };
  
  let addDeviceValue = '';

  const statusColors = ['bg-green-500', 'bg-orange-400', 'bg-firewall-red'];
  const statusLabels = ['ON', 'OFF'];
              
  interface Device {
    name: string;
    mac: string;
    status: boolean;
    smoke: boolean;
  }

  export let data;

  $: ({ user, supabase, devices } = data);

  let addDeviceForm: HTMLFormElement;

  async function addDevice() {
    console.log(addDeviceValue);
    const formData = new FormData(addDeviceForm);
    const macAddress = formData.get('macAddress') as string;

    console.log(macAddress);

    const { data, error } = await supabase
      .from('devices')
      .insert({
        user: user?.id,
        mac: macAddress
      })
      .select();

    if (error) {
      console.error(error);
    } else {
      console.log(data);
    }
  }

  async function toggleDevice(device: Device) {
    const { data, error } = await supabase
      .from('actions')
      .insert({
        mac: device.mac,
        action: device.status ? 'OFF' : 'ON'
      })
      .select();
    
    if (error) {
      console.error(error);
    } else {
      console.log(data);
    }

    device.status = !device.status;
    devices = devices;
  }
</script>

<nav class='grid grid-flow-row grid-cols-2 justify-between h-min w-auto mb-6 mx-6'>
  <p class='text-2xl text-firewall-red font-kollektif'>FireWall</p>
  <div class='text-mg text-black italic text-right font-DM'>
    Hello, user!
  </div>
</nav>
<div class='grid grid-flow-row grid-cols-1 lg:grid-cols-2 gap-4 overflow-y-scroll mx-6'>
  <div class='flex flex-row justify-between w-full h-min lg:col-span-2 items-center'>
    <h1 class='text-3xl font-kollektif'>Devices</h1>
    <form bind:this={addDeviceForm} on:submit|preventDefault={addDevice}>
      <label for="macAddress">
        <input name="macAddress" type="hidden" bind:value={ addDeviceValue }/>
      </label>
      <button class='bg-firewall-red p-4 rounded-lg w-40 h-min text-sm text-white font-DM' on:click={() => { modalStore.trigger(modalAddDevice); }}>+ Add Device</button>
    </form>
  </div>
  {#each devices as device (device.id)}
    <div class='flex flex-row flex-none w-auto h-40 p-8 items-center bg-gray-200 rounded-lg border border-gray-200 font-DM gap-6 justify-between'>
      <p>plug img</p>
      <div class='h-auto w-1/4 grid grid-flow-cols justify-items-left justify-self-center'>
        <p class='w-min h-min'>{ device.name }</p>
        <p class='w-min h-min'>{ device.mac }</p>
      </div>
      <button class='h-full w-1/4 { statusColors[device.status] } text-white rounded-lg' on:click={() => toggleDevice(device)}>{ statusLabels[device.status] }</button>
    </div>
  {/each}
</div>

<!-- top bar -->
    <!-- FireWall name -->
    <!-- user icon -->
        <!-- logout -->
<!-- devices -->
    <!-- title -->
        <!-- add device -->
    <!-- device list -->
        <!-- smoke detected -->
        <!-- plug icon -->
        <!-- name, number -->
        <!-- status -->
